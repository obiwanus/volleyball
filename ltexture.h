#ifndef LTEXTURE_H
#define LTEXTURE_H


// Texture wrapper class
class LTexture
{
public:
    LTexture();
    ~LTexture();

    bool loadFromFile(std::string path);
    void free();
    void render(int x, int y, SDL_Rect* clip = NULL);

    int mWidth;
    int mHeight;

private:
    SDL_Texture* mTexture;
};


#endif // LTEXTURE_H
