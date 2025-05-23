#include "TextRenderer.hpp"

TextRenderer::TextRenderer() {

}

TextRenderer::~TextRenderer() {

}

void TextRenderer::Init(const char* filepath, const int& fontSize) {
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		exit(1);
	}

	if (FT_New_Face(ft, filepath, 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		exit(1);
	}
	else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, fontSize * sizeMultiplier);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);

	}
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void TextRenderer::SetFontSize(const int& fontSize) {
	FT_Set_Pixel_Sizes(face, 0, fontSize);
}

void TextRenderer::RenderText(ShaderProgram* shader, GLuint& pipelineProgramID, std::string text, float x, float y, float scale, glm::vec3 color, glm::mat4 projectionMatrix, glm::mat4 modelMatrix, bool centeredX, bool centeredY, bool rightSided) {
    // this implementation is not the most efficient, but since its not a bottleneck, there's no reason to optimize it right now. Will do it later
    
    // activate corresponding render state	
    float xtemp = x;
    float xtemp2 = x;
    float tempy = 0.0f;
    scale = scale / sizeMultiplier;
    shader->UseInPipeline(pipelineProgramID);
    shader->SetVec3("uTextColor", color);
    shader->SetMat4("uProjection", projectionMatrix);
    shader->SetMat4("uModelMatrix", modelMatrix);

    std::string::const_iterator r;
    for (r = text.begin(); r != text.end(); r++) {
        Character ch = Characters[*r];
        xtemp2 += (ch.Advance >> 6) * scale;
        tempy = ch.Bearing.y * scale;
    }
    // translate and divide by 2, centered, divide by 1 (no division) - right sided, no translate - left sided

    if (centeredX) {
        modelMatrix = glm::translate(modelMatrix, glm::vec3((xtemp - xtemp2) / 2, 0.0f, 0.0f));
    }
    if (centeredY) {
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -tempy / 2, 0.0f));
    }
    if (rightSided && !centeredY) {
        modelMatrix = glm::translate(modelMatrix, glm::vec3((xtemp - xtemp2), -tempy, 0.0f));
    }
    else if (rightSided && centeredY) {
        modelMatrix = glm::translate(modelMatrix, glm::vec3((xtemp - xtemp2), 0.0f, 0.0f));
    }
    shader->SetMat4("uModelMatrix", modelMatrix);
    

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}