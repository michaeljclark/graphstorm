// See LICENSE for license details.

#include "EG.h"
#include "EGThread.h"
#include "EGImage.h"
#include "EGResource.h"

static const char* class_name = "EGImageDecompressor";

static EGbool debugMode = false;


/* EGImageDecompressor */

EGImageDecompressor::EGImageDecompressor() : queueMutex(), queueCondVar(queueMutex), itemtodecompress(), itemdecompressing(), itemtoprocess()
{
    if (start() < 0) {
        EGError("%s:%s couldn't start image decompression thread\n", class_name, __func__);
    }
}

EGThreadExitCode EGImageDecompressor::run()
{
    EGImageDecompressItemList::iterator di;
    
    EGDebug("%s:%s decompress thread started\n", class_name, __func__);
    queueMutex.lock();
    while (true) {
        queueCondVar.wait();
        if (debugMode) {
            EGDebug("%s:%s decompress thread woke up: todecompress=%d decompressing=%d toprocess=%d\n", class_name, __func__,
                    (EGint)itemtodecompress.size(), (EGint)itemdecompressing.size(), (EGint)itemtoprocess.size());
        }
        // decompress an item
        di = itemdecompressing.begin();
        if (di != itemdecompressing.end()) {
            EGImageDecompressItemPtr item = *di;
            queueMutex.unlock();
            
            EGImagePtr image = EGImage::createFromResource(item->rsrc, NULL, item->cacheImage);
            if (!image) {
                EGError("%s:%s couldn't decompress image: %s\n", class_name, __func__, item->rsrc->getPath().c_str());
            }
            
            queueMutex.lock();
            item->image = image;
            di = itemdecompressing.erase(di);
            itemtoprocess.push_back(item);
        }
        if (debugMode) {
            EGDebug("%s:%s decompress thread going to sleep: todecompress=%d decompressing=%d toprocess=%d\n", class_name, __func__,
                    (EGint)itemtodecompress.size(), (EGint)itemdecompressing.size(), (EGint)itemtoprocess.size());
        }
    }
    
    EGDebug("%s:%s exiting\n", class_name, __func__);
    return 0;
}

void EGImageDecompressor::queueItem(EGImageDecompressItemPtr decompressItem)
{
    queueMutex.lock();
    if (decompressItem->cacheImage) {
        EGImageDecompressItemMap::iterator existdi;
        if ((existdi = imagedecompressmap.find(decompressItem->rsrc->getPath())) != imagedecompressmap.end()) {
            (*existdi).second->waiters.push_back(decompressItem);
            queueMutex.unlock();
            return;
        }
    }
    itemtodecompress.push_back(decompressItem);
    imagedecompressmap[decompressItem->rsrc->getPath()] = decompressItem;
    queueMutex.unlock();
}

EGint EGImageDecompressor::idleProcessItems()
{
    EGImageDecompressItemList::iterator di;

    // put image in decompressing queue if the decompress thread is not busy
    queueMutex.lock();
    if (!(itemdecompressing.size() || itemtoprocess.size())) {
        for (di = itemtodecompress.begin(); di != itemtodecompress.end(); di++) {
            EGImageDecompressItemPtr item = *di;
            di = itemtodecompress.erase(di);
            itemdecompressing.push_back(item);
            queueCondVar.signal();
            queueMutex.unlock();
            return false;
        }
    }
    
    // signal to decompress thread to grab item from queue
    if (itemdecompressing.size()) {
        queueCondVar.signal();
    }
    
    // process an image if decompression is complete
    for (di = itemtoprocess.begin(); di != itemtoprocess.end(); di++) {
        EGImageDecompressItemPtr item = *di;
        if (item->image) {
            item->image->createGLTexture();
            item->decompressCompleted();
            for (EGImageDecompressItemList::iterator wdi = item->waiters.begin(); wdi != item->waiters.end(); wdi++) {
                (*wdi)->image = item->image;
                (*wdi)->decompressCompleted();
            }
        } else {
            item->decompressFailed();
            for (EGImageDecompressItemList::iterator wdi = item->waiters.begin(); wdi != item->waiters.end(); wdi++) {
                (*wdi)->decompressFailed();
            }
        }
        di = itemtoprocess.erase(di);
        imagedecompressmap.erase(item->rsrc->getPath());
        queueMutex.unlock();
        return true;
    }
    queueMutex.unlock();
    
    return false;
}


/* EGImageDecompressItem */

EGImageDecompressItem::EGImageDecompressItem(EGResourcePtr rsrc, EGbool cacheImage) : rsrc(rsrc), image(), cacheImage(cacheImage), waiters() {}

EGImageDecompressItem::~EGImageDecompressItem() {}
