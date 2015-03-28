/*
 *  EWWidgetRendererES2.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EWWidgetRendererES2_H
#define EWWidgetRendererES2_H

/* EWWidgetRendererES2 */

class EWWidgetRendererES2 : public EWWidgetRenderer
{
protected:
    EGRenderProgramPtr notex_program;
    EGRenderAttributeInfo* notex_a_position;
    EGRenderAttributeInfo* notex_a_color;
    EGRenderProgramPtr tex_program;
    EGRenderUniformInfo* tex_u_texture0;
    EGRenderAttributeInfo* tex_a_position;
    EGRenderAttributeInfo* tex_a_color;
    EGRenderAttributeInfo* tex_a_texcoord0;
    EGbool scissorEnabled;
    EGRenderBatchList batchList;
        
    template <typename T> T* lastBatch()
    {
        if (batchList.begin() != batchList.end()) {
            EGRenderBatchPtr &batch = batchList.back();
            if (typeid(*batch) == typeid(T)) return static_cast<T*>(batch.get());
        }
        return NULL;
    }

    template <typename T> T* newBatch()
    {
        T *batch = new T(widget->getName());
        batchList.push_back(EGRenderBatchPtr(batch));
        return batch;
    }

public:
    EWWidgetRendererES2(EWWidget *widget);    

    void draw();
    void begin();
    void end();
    void scissor(EGRect rect);
    void fill(EGRect rect, EGColor color);
    void fill(EGRect outerRect, EGRect innerRect, EGColor color);
    void fill(EGPointList poly, EGColor color);
    void stroke(EGRect rect, EGColor color, EGfloat width, EWStrokeRectFlags flags);
    void stroke(EGLine line, EGColor color, EGfloat width);
    void stroke(EGPointList poly, EGColor color, EGfloat width, EGbool closed);
    void paint(EGRect rect, EGImagePtr image, EGColor color, EGfloat *uvs);
    void paint(EGPoint point, EGTextPtr text, EGColor color);
};

#endif
