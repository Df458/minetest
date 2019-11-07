/*
Minetest
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "guiButtonImage.h"

#include "client/guiscalingfilter.h"
#include "debug.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "StyleSpec.h"

using namespace irr;
using namespace gui;

GUIButtonImage::GUIButtonImage(gui::IGUIEnvironment *environment, gui::IGUIElement *parent,
		s32 id, core::rect<s32> rectangle, bool noclip)
	: GUIButton (environment, parent, id, rectangle, noclip)
{
}

void GUIButtonImage::drawContent()
{
	GUISkin *skin = dynamic_cast<GUISkin *>(Environment->getSkin());
	video::IVideoDriver *driver = Environment->getVideoDriver();

	EGUI_BUTTON_IMAGE_STATE imageState = getImageState(isPressed(), ButtonForegroundImages);
	video::ITexture *texture = ButtonForegroundImages[(u32)imageState].Texture;

	if (texture) {
		core::rect<s32> sourceRect(ButtonForegroundImages[(u32)imageState].SourceRect);
		if (sourceRect.getWidth() == 0 && sourceRect.getHeight() == 0) {
			sourceRect = core::rect<s32>(
					core::position2di(0,0),
					texture->getOriginalSize());
		}

		core::position2d<s32> pos(AbsoluteRect.getCenter());
		pos.X -= sourceRect.getWidth() / 2;
		pos.Y -= sourceRect.getHeight() / 2;

		core::rect<s32> rect = isScalingImage()
			? AbsoluteRect
			: core::rect<s32>(pos, sourceRect.getSize());

		if (isPressed() && isDrawingBorder()) {
			rect += core::dimension2d<s32>(
					skin->getSize(irr::gui::EGDS_BUTTON_PRESSED_IMAGE_OFFSET_X),
					skin->getSize(irr::gui::EGDS_BUTTON_PRESSED_IMAGE_OFFSET_Y));
		}

		driver->draw2DImage(ButtonForegroundImages[(u32)imageState].Texture,
				rect, sourceRect, &AbsoluteClippingRect,
				0, isAlphaChannelUsed());
	}
}

void GUIButtonImage::setForegroundImage(EGUI_BUTTON_IMAGE_STATE state,
		video::ITexture *image, const core::rect<s32>& sourceRect)
{
	if (state >= EGBIS_COUNT)
		return;

	if (image)
		image->grab();

	u32 stateIdx = (u32)state;
	if (ButtonForegroundImages[stateIdx].Texture)
		ButtonForegroundImages[stateIdx].Texture->drop();

	ButtonForegroundImages[stateIdx].Texture = image;
	ButtonForegroundImages[stateIdx].SourceRect = sourceRect;
}

void GUIButtonImage::setForegroundImage(video::ITexture *image)
{
	setForegroundImage(gui::EGBIS_IMAGE_UP, image);
}

void GUIButtonImage::setForegroundImage(video::ITexture *image, const core::rect<s32>& pos)
{
	setForegroundImage(gui::EGBIS_IMAGE_UP, image, pos);
}

void GUIButtonImage::setPressedForegroundImage(video::ITexture *image)
{
	setForegroundImage(gui::EGBIS_IMAGE_DOWN, image);
}

void GUIButtonImage::setPressedForegroundImage(video::ITexture *image, const core::rect<s32>& pos)
{
	setForegroundImage(gui::EGBIS_IMAGE_DOWN, image, pos);
}

void GUIButtonImage::setHoveredForegroundImage(video::ITexture *image)
{
	setForegroundImage(gui::EGBIS_IMAGE_UP_MOUSEOVER, image);
	setForegroundImage(gui::EGBIS_IMAGE_UP_FOCUSED_MOUSEOVER, image);
}

void GUIButtonImage::setHoveredForegroundImage(video::ITexture *image, const core::rect<s32>& pos)
{
	setForegroundImage(gui::EGBIS_IMAGE_UP_MOUSEOVER, image, pos);
	setForegroundImage(gui::EGBIS_IMAGE_UP_FOCUSED_MOUSEOVER, image, pos);
}

void GUIButtonImage::setFromStyle(const StyleSpec& style, ISimpleTextureSource *tsrc)
{
	GUIButton::setFromStyle(style, tsrc);

	video::IVideoDriver *driver = Environment->getVideoDriver();

	if (style.isNotDefault(StyleSpec::FGIMG)) {
		video::ITexture *texture = style.getTexture(StyleSpec::FGIMG, tsrc);
		video::ITexture *hovered_texture = style.getTexture(StyleSpec::FGIMG_HOVERED, tsrc, texture);
		video::ITexture *pressed_texture = style.getTexture(StyleSpec::FGIMG_PRESSED, tsrc, texture);

		const core::position2di buttonCenter(AbsoluteRect.getCenter());
		core::position2d<s32> geom(buttonCenter);

		setForegroundImage(guiScalingImageButton(driver, texture, geom.X, geom.Y));
		setHoveredForegroundImage(guiScalingImageButton(driver, hovered_texture, geom.X, geom.Y));
		setPressedForegroundImage(guiScalingImageButton(driver, pressed_texture, geom.X, geom.Y));
		setScaleImage(true);
	}
}

GUIButtonImage *GUIButtonImage::addButton(IGUIEnvironment *environment,
		const core::rect<s32>& rectangle, IGUIElement *parent,
		s32 id, const wchar_t *text, const wchar_t *tooltiptext)
{
	GUIButtonImage *button = new GUIButtonImage(environment,
			parent ? parent : environment->getRootGUIElement(), id, rectangle);

	if (text)
		button->setText(text);

	if (tooltiptext)
		button->setToolTipText (tooltiptext);

	button->drop();
	return button;
}
