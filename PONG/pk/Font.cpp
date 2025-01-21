#include "Font.h"
#include "Shader.h"

#include <glad/glad.h>

Font::Font(const std::string& _Path, const std::string& _Name, const glm::mat4& _Projection, const Shader::SharedPtr& _TextShader)
	: Path(_Path), Name(_Name), Size(14), Projection(_Projection), TextShader(_TextShader)
{
    if (TextShader == nullptr)
    {
        throw LoadError("ERROR::FONT TextShader empty");
    }

    TextShader->Use();
    TextShader->SetMatrix("projection", Projection);

    PrepareRenderQuad();
}

std::string Font::GetName() const
{
	return Name;
}

std::string Font::GetPath() const
{
	return Path;
}

unsigned int Font::GetSize() const
{
    return Size;
}

void Font::Load(unsigned int _Size)
{
    Characters.clear();

    FT_Library FontLibrary;
    if (FT_Init_FreeType(&FontLibrary))
    {
        throw LoadError("ERROR::FREETYPE: Could not init FreeType Library");
    }

    FT_Face FontFace;
    if (FT_New_Face(FontLibrary, Path.c_str(), 0, &FontFace))
    {
        throw LoadError("ERROR::FREETYPE: Failed to load font");
    }

    Size = _Size;
    FT_Set_Pixel_Sizes(FontFace, 0, Size);
    LoadCharacters(FontFace);

    FT_Done_Face(FontFace);
    FT_Done_FreeType(FontLibrary);

    bLoaded = true;
}

void Font::Render(const std::string& Text, const glm::vec2& Position, float Scale, const float Color[])
{
    if (!bLoaded)
    {
        return;
    }

    TextShader->Use();
    TextShader->SetColor("textColor", Color);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(QuadId);

    // iterate through all characters
    float x = Position.x;
    float y = Position.y;

    Character MaxChar = Characters['H'];

    for (const char c : Text)
    {
        Character Glyph = Characters[c];

        float xpos = x + Glyph.Bearing.x * Scale;
        float ypos = y + (MaxChar.Bearing.y - Glyph.Bearing.y) * Scale;

        float w = Glyph.Size.x * Scale;
        float h = Glyph.Size.y * Scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, Glyph.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, BufferId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (Glyph.Advance >> 6) * Scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Font::LoadCharacters(FT_Face& Face)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(Face, c, FT_LOAD_RENDER))
        {
            continue;
        }

        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            Face->glyph->bitmap.width,
            Face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            Face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(Face->glyph->bitmap.width, Face->glyph->bitmap.rows),
            glm::ivec2(Face->glyph->bitmap_left, Face->glyph->bitmap_top),
            Face->glyph->advance.x
        };

        Characters.insert(std::pair<char, Character>(c, character));
    }
}

void Font::PrepareRenderQuad()
{
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    QuadId = VAO;
    BufferId = VBO;
}

