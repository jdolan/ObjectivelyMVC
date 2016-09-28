/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
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

#include <Objectively/Config.h>
#include <Objectively/String.h>

#include <ObjectivelyMVC/TextView.h>

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
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	TextView *this = (TextView *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("defaultText", InletTypeCharacters, &this->defaultText, NULL),
		MakeInlet("isEditable", InletTypeBool, &this->isEditable, NULL)
	);

	$(self, bind, dictionary, inlets);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((TextView *) self, initWithFrame, NULL, ControlStyleDefault);
}

/**
 * @see View::render(View *, Renderer *)
 */
static void render(View *self, Renderer *renderer) {
	
	super(View, self, render, renderer);
	
	TextView *this = (TextView *) self;
	
	const char *text = this->attributedText->string.chars;
	
	if (text == NULL || strlen(text) == 0) {
		if ($((Control *) this, focused) == false) {
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
	
	if ($((Control *) this, focused)) {
		text = text ?: "";
		
		int w, h;
		if (this->position == this->attributedText->string.length) {
			$(this->text->font, sizeCharacters, text, &w, &h);
		} else {
//			char *chars = strndup(text, this->position);
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
static _Bool captureEvent(Control *self, const SDL_Event *event) {
	
	_Bool didEdit = false, didCaptureEvent = false;

	TextView *this = (TextView *) self;
	if (this->isEditable) {
		if (event->type == SDL_MOUSEBUTTONDOWN) {
			if ($((View *) self, didReceiveEvent, event)) {
				if ((self->state & ControlStateFocused) == 0) {
					self->state |= ControlStateFocused;
					SDL_StartTextInput();
					if (this->delegate.didBeginEditing) {
						this->delegate.didBeginEditing(this);
					}
				}
				didCaptureEvent = true;
			} else {
				if (self->state & ControlStateFocused) {
					self->state &= ~ControlStateFocused;
					SDL_StopTextInput();
					if (this->delegate.didEndEditing) {
						this->delegate.didEndEditing(this);
					}
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
						if (this->delegate.didEndEditing) {
							this->delegate.didEndEditing(this);
						}
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
						
					default:
						break;
				}
			}
		}
		
		if (didEdit) {
			if (this->delegate.didEdit) {
				this->delegate.didEdit(this);
			}
		}
	}
	
	return didCaptureEvent;
}

#pragma mark - TextView

/**
 * @fn TextView *TextView::initWithFrame(TextView *self, const SDL_Rect *frame, ControlStyle style)
 *
 * @memberof TextView
 */
static TextView *initWithFrame(TextView *self, const SDL_Rect *frame, ControlStyle style) {

	self = (TextView *) super(Control, self, initWithFrame, frame, style);
	if (self) {
		self->attributedText = $$(MutableString, string);
		assert(self->attributedText);
		
		self->isEditable = true;
		
		self->text = $(alloc(Text), initWithText, NULL, NULL);
		assert(self->text);
		
		$((View *) self, addSubview, (View *) self->text);

		self->control.view.clipsSubviews = true;
		
		if (self->control.style == ControlStyleDefault) {
			self->control.bevel = ControlBevelTypeInset;
			
			self->control.view.backgroundColor = Colors.DimGray;
			
			if (self->control.view.frame.w == 0) {
				self->control.view.frame.w = DEFAULT_TEXTVIEW_WIDTH;
			}
		}
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->interface)->init = init;
	((ViewInterface *) clazz->interface)->render = render;
	
	((ControlInterface *) clazz->interface)->captureEvent = captureEvent;
	
	((TextViewInterface *) clazz->interface)->initWithFrame = initWithFrame;
}

Class _TextView = {
	.name = "TextView",
	.superclass = &_Control,
	.instanceSize = sizeof(TextView),
	.interfaceOffset = offsetof(TextView, interface),
	.interfaceSize = sizeof(TextViewInterface),
	.initialize = initialize,
};

#undef _Class
