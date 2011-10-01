#include "drawhelper.h"

void drawSprite(SDL_Surface* target, SDL_Surface* sprite, unsigned int x, unsigned int y){
  { // draw eevee on screen?
    SDL_Rect DestR;

    { // destination / where to draw
      DestR.x = x;
      DestR.y = y;
    }

    SDL_Rect SrcR;
    { // source rectangle - if not all should be printed

      SrcR.x = 5;
      SrcR.y = 5;
      SrcR.w = 5;
      SrcR.h = 5;
    }
    
    SDL_BlitSurface(sprite, NULL, target, &DestR);
  }
}


void writeOnSurface(
  SDL_Surface* target,
  const std::string& writeString,
  TTF_Font* font,
  SDL_Color color,
  unsigned int x, unsigned int y
  )
{
  
  SDL_Surface *text_surface;
  if(!(text_surface=TTF_RenderUTF8_Solid(font,writeString.c_str(),color))) {
    //handle error here, perhaps print TTF_GetError at least
  } else {

    SDL_Rect DestR;

    { // destination / where to draw
      DestR.x = x;
      DestR.y = y;
    }
    SDL_BlitSurface(text_surface,NULL,target,&DestR);
    //perhaps we can reuse it, but I assume not for simplicity.
    SDL_FreeSurface(text_surface);
  }
}

void writeOnSurfaceCentered(
  SDL_Surface* target,
  const std::string writeString,
  TTF_Font* font,
  SDL_Color color,
  SDL_Rect screenArea
  )
{
  int renderedWidth;
  int renderedHeight;
    
  TTF_SizeUTF8(font, writeString.c_str(), &renderedWidth, &renderedHeight);
  
  int emptyWidth  = (screenArea.w - renderedWidth);
  int emptyHeight = (screenArea.h - renderedHeight);
  
  int outputStartLine = screenArea.y + (emptyHeight/ 2);
  int outputStartColumn = screenArea.x + (emptyWidth / 2);
  writeOnSurface(
    target, writeString,font,color,
    outputStartColumn,outputStartLine
    );
}
void writeOnSurfaceVCentered(
  SDL_Surface* target,
  const std::string writeString,
  TTF_Font* font,
  SDL_Color color,
  SDL_Rect screenArea
  )
{
  int renderedHeight;
    
  TTF_SizeUTF8(font, writeString.c_str(), NULL, &renderedHeight);
  
  int emptyHeight = (screenArea.h - renderedHeight);
  
  int outputStartLine = screenArea.y + (emptyHeight / 2);
  int outputStartColumn = screenArea.x;
  writeOnSurface(
    target, writeString,font,color,
    outputStartColumn,outputStartLine
    );
}

void paintRectangleOnSurface(
  SDL_Surface* target,
  SDL_Rect screenArea,
  SDL_Color color
  )
{
  uint32_t mappedColor = SDL_MapRGB(target->format,color.r,color.g,color.b);
  SDL_FillRect(target,&screenArea,mappedColor);
}

