// See LICENSE for license details.

#include "EG.h"
#include "NSStubs.h"

std::string NSStubs::getTempDir()
{
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
    NSString* tempDir = NSTemporaryDirectory();
    std::string path([tempDir UTF8String]);
    [autoreleasepool release];
    return path;
}

std::string NSStubs::getHomeDir()
{
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
    NSString* tempDir = NSHomeDirectory();
    std::string path([tempDir UTF8String]);
    [autoreleasepool release];
    return path;
}

std::string NSStubs::getPublicDir()
{
    NSAutoreleasePool *autoreleasepool = [[NSAutoreleasePool alloc] init];
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *publicDocumentsDir = [paths objectAtIndex:0]; 
    std::string path([publicDocumentsDir UTF8String]);
    [autoreleasepool release];
    return path;
}

std::string NSStubs::localizedString(std::string key)
{
    return std::string([NSLocalizedString([NSString stringWithUTF8String:key.c_str()], nil) UTF8String]);
}
