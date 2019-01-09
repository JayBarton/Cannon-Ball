/*******************************************************************
** Modified from code written by Joey de Vries 
** https://learnopengl.com/
** https://twitter.com/JoeyDeVriez
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include <iostream>

#include "Texture.h"


Texture2D::Texture2D()
    : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
{
    glGenTextures(1, &this->ID);
}

void Texture2D::Generate(GLuint width, GLuint height, unsigned char* data)
{
    this->Width = width;
    this->Height = height;
    // Create Texture
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
    // Set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}

std::vector<glm::vec4> Texture2D::GetUVs(int w, int h)
{
    std::vector<glm::vec4> uvs;
    int rows = Width/ w;
    int columns = Height / h;
    for(int c = 0; c < columns; c ++)
    {
        for(int i = 0; i < rows; i ++)
        {
            //The 0.5 is to prevent pixel bleeding. I don't entirely understand it honestly.
            uvs.emplace_back(glm::vec4(float(((i) * w + 0.5f))/Width,
                                  float(((1 + i) * w - 0.5f))/Width,
                                  float(((c) * h + 0.5f))/Height,
                                  float(((1 + c) * h - 0.5f))/Height));
        }
    }
    return uvs;
}

std::vector<glm::vec4> Texture2D::GetUVs(int startX, int startY, int w, int h, int rows, int columns)
{
    std::vector<glm::vec4> uvs;
//    h += startY;
  //  w += startX;

    for(int c = 0; c < columns; c ++)
    {
        for(int i = 0; i < rows; i ++)
        {
            //The 0.5 is to prevent pixel bleeding. I don't entirely understand it honestly.
            uvs.emplace_back(glm::vec4((float((((i) * w + 0.5f))) + startX)/Width,
                                  float(((((1 + i) * w - 0.5f))) + startX)/Width,
                                  float(((((c) * h + 0.5f))) + startY)/Height,
                                  float(((((1 + c) * h - 0.5f))) + startY)/Height));
        }
    }
    return uvs;
}
