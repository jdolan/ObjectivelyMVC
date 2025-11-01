/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL2 and GNU C.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <assert.h>
#include <string.h>

#include <Objectively/String.h>

#include "TextView.h"

#define _Class _TextView

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  TextView *this = (TextView *) self;

  free(this->defaultText);

  release(this->text);

  release(this->attributedText);

  super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @fn void View::applyStyle(View *self, const Style *style)
 * @memberof View
 */
static void applyStyle(View *self, const Style *style) {

  super(View, self, applyStyle, style);

  TextView *this = (TextView *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("editable", InletTypeBool, &this->isEditable, NULL)
  );

  $(self, bind, inlets, style->attributes);
}

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

  super(View, self, awakeWithDictionary, dictionary);

  TextView *this = (TextView *) self;

  const Inlet inlets[] = MakeInlets(
    MakeInlet("defaultText", InletTypeCharacters, &this->defaultText, NULL)
  );

  $(self, bind, inlets, dictionary);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
  return (View *) $((TextView *) self, initWithFrame, NULL);
}

/**
 * @see View::layoutSubviews(View *)
 */
static void layoutSubviews(View *self) {

  TextView *this = (TextView *) self;

  const char *text = this->attributedText->string.chars;

  if (text == NULL || strlen(text) == 0) {
    if ($((Control *) this, isFocused) == false) {
      text = this->defaultText;
    }
  }

  if (text == NULL) {
    $(this->text, setText, NULL);
  } else {
    if (this->text->text) {
      if (strcmp(text, this->text->text)) {
        $(this->text, setText, text);
      }
    } else {
      $(this->text, setText, text);
    }
  }

  $((View *) this->text, layoutIfNeeded);

  super(View, self, layoutSubviews);
}

/**
 * @see View::render(View *, Renderer *)
 */
static void render(View *self, Renderer *renderer) {

  super(View, self, render, renderer);

  TextView *this = (TextView *) self;

  if ($((Control *) this, isFocused)) {
    const char *text = this->text->text ?: "";

    int w, h;
    if (this->position == strlen(text)) {
      $(this->text->font, sizeCharacters, text, &w, &h);
    } else {
      char *chars = calloc(this->position + 1, sizeof(char));
      strncpy(chars, text, this->position);

      $(this->text->font, sizeCharacters, chars, &w, &h);
      free(chars);
    }

    SDL_Rect frame = $((View *) this->text, renderFrame);

    const SDL_Point points[] = {
      { frame.x + w, frame.y },
      { frame.x + w, frame.y + h }
    };

    $(renderer, drawLine, points);
  }
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static bool captureEvent(Control *self, const SDL_Event *event) {

  bool didEdit = false, didBeginEditing = false, didEndEditing = false, didCaptureEvent = false;

  TextView *this = (TextView *) self;
  if (this->isEditable) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
      if ($((View *) self, didReceiveEvent, event)) {
        if ((self->state & ControlStateFocused) == 0) {
          self->state |= ControlStateFocused;
          SDL_StartTextInput();
          didBeginEditing = true;
        }
        didCaptureEvent = true;
      } else {
        if (self->state & ControlStateFocused) {
          self->state &= ~ControlStateFocused;
          SDL_StopTextInput();
          didEndEditing = true;
          didCaptureEvent = true;
        }
      }
    } else if (event->type == SDL_TEXTINPUT) {
      if (self->state & ControlStateFocused) {
        if (this->position == this->attributedText->string.length) {
          $(this->attributedText, appendCharacters, event->text.text);
        } else {
          $(this->attributedText, insertCharactersAtIndex, event->text.text, this->position);
        }
        this->position += strlen(event->text.text);
        didEdit = didCaptureEvent = true;
      }
    } else if (event->type == SDL_KEYDOWN) {
      if (self->state & ControlStateFocused) {
        didCaptureEvent = true;

        const char *chars = this->attributedText->string.chars;
        const size_t len = this->attributedText->string.length;

        switch (event->key.keysym.sym) {

          case SDLK_ESCAPE:
          case SDLK_KP_ENTER:
          case SDLK_KP_TAB:
          case SDLK_RETURN:
          case SDLK_TAB:
            self->state &= ~ControlStateFocused;
            SDL_StopTextInput();
            didEndEditing = true;
            break;

          case SDLK_BACKSPACE:
          case SDLK_KP_BACKSPACE:
            if (this->position > 0) {
              const Range range = { .location = this->position - 1, .length = 1 };
              $(this->attributedText, deleteCharactersInRange, range);
              this->position--;
              didEdit = true;
            }
            break;

          case SDLK_DELETE:
            if (this->position < len) {
              const Range range = { .location = this->position, .length = 1 };
              $(this->attributedText, deleteCharactersInRange, range);
              didEdit = true;
            }
            break;

          case SDLK_LEFT:
            if (SDL_GetModState() & KMOD_CTRL) {
              while (this->position > 0 && chars[this->position] == ' ') {
                this->position--;
              }
              while (this->position > 0 && chars[this->position] != ' ') {
                this->position--;
              }
            } else if (this->position > 0) {
              this->position--;
            }
            break;

          case SDLK_RIGHT:
            if (SDL_GetModState() & KMOD_CTRL) {
              while (this->position < len && chars[this->position] == ' ') {
                this->position++;
              }
              while (this->position < len && chars[this->position] != ' ') {
                this->position++;
              }
              if (this->position < len) {
                this->position++;
              }
            } else if (this->position < len) {
              this->position++;
            }
            break;

          case SDLK_HOME:
            this->position = 0;
            break;

          case SDLK_END:
            this->position = len;
            break;

          case SDLK_a:
            if (SDL_GetModState() & KMOD_CTRL) {
              this->position = 0;
            }
            break;
          case SDLK_e:
            if (SDL_GetModState() & KMOD_CTRL) {
              this->position = len;
            }
            break;

          case SDLK_v:
            if ((SDL_GetModState() & (KMOD_CTRL | KMOD_GUI)) && SDL_HasClipboardText()) {
              const char *text = SDL_GetClipboardText();
              if (this->position == len) {
                $(this->attributedText, appendCharacters, text);
              } else {
                $(this->attributedText, insertCharactersAtIndex, text, this->position);
              }
              this->position += strlen(text);
              didEdit = true;
            }
            break;
        }
      }
    }

    if (didBeginEditing) {
      if (this->delegate.didBeginEditing) {
        this->delegate.didBeginEditing(this);
      }
    }

    if (didEdit) {
      self->view.needsLayout = true;
      if (this->delegate.didEdit) {
        this->delegate.didEdit(this);
      }
      
      $((View *) self, emitViewEvent, ViewEventChange, NULL);
    }

    if (didEndEditing) {
      if (this->delegate.didEndEditing) {
        this->delegate.didEndEditing(this);
      }
    }
  }

  if (didCaptureEvent) {
    return true;
  }
  
  return super(Control, self, captureEvent, event);
}

#pragma mark - TextView

/**
 * @fn TextView *TextView::initWithFrame(TextView *self, const SDL_Rect *frame)
 * @memberof TextView
 */
static TextView *initWithFrame(TextView *self, const SDL_Rect *frame) {

  self = (TextView *) super(Control, self, initWithFrame, frame);
  if (self) {
    self->attributedText = $$(MutableString, string);
    assert(self->attributedText);

    self->isEditable = true;

    self->text = $(alloc(Text), initWithText, NULL, NULL);
    assert(self->text);

    $((View *) self, addSubview, (View *) self->text);

    self->control.view.clipsSubviews = true;
  }

  return self;
}

/**
 * @fn void TextView::setAttributedText(TextView *self, const char *attributedText)
 * @memberof TextView
 */
static void setAttributedText(TextView *self, const char *attributedText) {

  if (strcmp(self->attributedText->string.chars ?: "", attributedText ?: "")) {

    $(self->attributedText, setCharacters, attributedText);

    self->position = self->attributedText->string.length;

    self->control.view.needsLayout = true;
  }
}

/**
 * @fn void TextView::setDefaultText(TextView *self, const char *defaultText)
 * @memberof TextView
 */
static void setDefaultText(TextView *self, const char *defaultText) {

  if (strcmp(self->defaultText ?: "", defaultText ?: "")) {

    free(self->defaultText);

    if (defaultText) {
      self->defaultText = strdup(defaultText);
    } else {
      self->defaultText = NULL;
    }

    self->control.view.needsLayout = true;
  }
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ViewInterface *) clazz->interface)->applyStyle = applyStyle;
  ((ViewInterface *) clazz->interface)->awakeWithDictionary = awakeWithDictionary;
  ((ViewInterface *) clazz->interface)->init = init;
  ((ViewInterface *) clazz->interface)->layoutSubviews = layoutSubviews;
  ((ViewInterface *) clazz->interface)->render = render;

  ((ControlInterface *) clazz->interface)->captureEvent = captureEvent;

  ((TextViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
  ((TextViewInterface *) clazz->interface)->setAttributedText = setAttributedText;
  ((TextViewInterface *) clazz->interface)->setDefaultText = setDefaultText;
}

/**
 * @fn Class *TextView::_TextView(void)
 * @memberof TextView
 */
Class *_TextView(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "TextView",
      .superclass = _Control(),
      .instanceSize = sizeof(TextView),
      .interfaceOffset = offsetof(TextView, interface),
      .interfaceSize = sizeof(TextViewInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
