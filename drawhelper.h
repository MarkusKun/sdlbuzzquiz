#ifndef _DRAWHELPER_H
#define _DRAWHELPER_H

#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

//! Todo: Generalize
void drawSprite(SDL_Surface* target, SDL_Surface* sprite, unsigned int x, unsigned int y);

//! simply write at defined point
void writeOnSurface(
  SDL_Surface* target,
  const std::string& writeString,
  TTF_Font* font,
  SDL_Color color,
  unsigned int x, unsigned int y
  );

//! centers horizontally and vertically
void writeOnSurfaceCentered(
  SDL_Surface* target,
  const std::string writeString,
  TTF_Font* font,
  SDL_Color color,
  SDL_Rect screenArea
  );

//! center vertically, not horizontally
void writeOnSurfaceVCentered(
  SDL_Surface* target,
  const std::string writeString,
  TTF_Font* font,
  SDL_Color color,
  SDL_Rect screenArea
  );

//! Paint the area with given color
void paintRectangleOnSurface(
  SDL_Surface* target,
  SDL_Rect screenArea,
  SDL_Color color
  );

#endif // _DRAWHELPER_H

