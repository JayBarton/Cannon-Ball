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

#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(Shader &shader)
{
    this->shader = shader;
    this->initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::DrawSprite(Texture2D &texture, glm::vec2 position, float rotate, float size, const glm::vec4 &color)
{
    // Prepare transformations
    this->shader.Use();
	glm::mat4 model;
     model = glm::translate(model, glm::vec3(position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

     model = glm::translate(model, glm::vec3(0.5f * size, 0.5f * size, 0.0f)); // Move origin of rotation to center of quad
     model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
     model = glm::translate(model, glm::vec3(-0.5f * size, -0.5f * size, 0.0f)); // Move origin back

     model = glm::scale(model, glm::vec3(size, size, 1.0f)); // Last scale
     
    this->shader.SetMatrix4("model", model);

    // Render textured quad
    this->shader.SetVector4f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SpriteRenderer::DrawSprite()
{
	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void SpriteRenderer::initRenderData()
{
    // Configure VAO/VBO

    GLfloat vertices[] =
    {
        // Tex
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

   // glBindBuffer(GL_ARRAY_BUFFER, VBO);
  //  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
   // glEnableVertexAttribArray(0);
  //  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void SpriteRenderer::setUVs(const glm::vec4& uv/* = glm::vec4((0,1,0,1)*/)
{

    GLfloat vertices[] =
    {
        // Pos      // Tex
        uv.x, uv.w,
        uv.y, uv.z,
        uv.x, uv.z,

        uv.x, uv.w,
        uv.y, uv.w,
        uv.y, uv.z
    };
	//this->shader.Use();
	 
	this->shader.Use().SetVector2fv("uvs", 12, vertices);
  /*  glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);*/
}
