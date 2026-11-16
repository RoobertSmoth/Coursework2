#include "spaceship.hpp"

#include <numbers>
#include <numbers>
#include <iostream>

#include <cmath>
#include <print>
#include <cassert>

#include <draw2d/draw.hpp>
#include <draw2d/image.hpp>
#include <draw2d/sampler.hpp>
#include <draw2d/surface.hpp>

#include <vmlib/transform2d.hpp>

// Line-based space ship
void SpaceShipLines::draw( Surface& aSurface, float aAngleRadians, Vec2f aTranslation ) const
{
	auto rot = make_rotation_2d( aAngleRadians );
	lines.draw( aSurface, color, rot, aTranslation );
}

SpaceShipLines make_spaceship_lines()
{
	static constexpr float xs[] = { 250.f, 200.f, 150.f, 100.f, 000.f, 040.f, -50.f, -140.f, -170.f };
	static constexpr float ys[] = { 190.f, 180.f, 70.f, 50.f, 30.f, 20.f };

	LineStrip spaceship{ { 
		{ 0.2f * xs[0], 0.2f * +ys[5] }, // upper half. starts at front, goes towards the back
		{ 0.2f * xs[1], 0.2f * +ys[3] },
		{ 0.2f * xs[2], 0.2f * +ys[3] },
		{ 0.2f * xs[3], 0.2f * +ys[4] }, 
		{ 0.2f * xs[4], 0.2f * +ys[4] },
		{ 0.2f * xs[4], 0.2f * +ys[2] },
		{ 0.2f * xs[5], 0.2f * +ys[1] },
		{ 0.2f * xs[6], 0.2f * +ys[0] },
		{ 0.2f * xs[8], 0.2f * +ys[2] },
		{ 0.2f * xs[7], 0.2f * +ys[3] },

		{ 0.2f * xs[7], 0.2f * -ys[3] }, // lower half, starts at the back and goes towards the front
		{ 0.2f * xs[8], 0.2f * -ys[2] }, // this is essentially the same as the upper half, except in reverse.
		{ 0.2f * xs[6], 0.2f * -ys[0] },
		{ 0.2f * xs[5], 0.2f * -ys[1] },
		{ 0.2f * xs[4], 0.2f * -ys[2] },
		{ 0.2f * xs[4], 0.2f * -ys[4] },
		{ 0.2f * xs[3], 0.2f * -ys[4] }, 
		{ 0.2f * xs[2], 0.2f * -ys[3] },
		{ 0.2f * xs[1], 0.2f * -ys[3] },
		{ 0.2f * xs[0], 0.2f * -ys[5] }, 

		{ 0.2f * xs[0], 0.2f * +ys[5] } // link back to beginning (connects both sides at the "front")
	} };

	return SpaceShipLines{ std::move(spaceship), { 0.2f, 0.4f, 0.7f } };
}

// Image atlas space ship
void SpaceShipAtlas::draw( Surface& aSurface, float aAngleRadians, Vec2f aTranslation ) const
{
	// TODO: Your implementation here
	//radians to degrees
	if (!atlas || std::isnan(aAngleRadians)) return; //seems to have fixed some issues
	float angleAdjust = -aAngleRadians; //+ static_cast<float>(M_PI); //rotates the other wayyyyyyy
	float twoPi = 2.f * static_cast<float>(M_PI); //two times pi lol, 360 degrees 
	angleAdjust = std::fmod( angleAdjust, twoPi );

	if( angleAdjust < 0.f )
		angleAdjust += twoPi;
	//working in radians now because degrees didnt work

	int index = static_cast<int>( std::round( angleAdjust * (64.f /twoPi)))%64;

	//find the actual image
	int gridX = 7 - (index % 8);
	int gridY = index / 8;

	//8 * 8 is 64, 64 images

	//find pixel coords me thinks
	Vec2i sourceSize;
	sourceSize.x =  atlas->get_width()/8;
	sourceSize.y = atlas->get_height()/(8);

	//this code below is old but i will keep it
	// sourceSize.x =  atlas->get_width()/atlasWidth;
	// sourceSize.y = atlas->get_height()/(atlasWidth);
	// sourceSize.y = atlas->get_height()/(atlasSize/atlasWidth);
	Vec2i aSourceOrigin;
	//rework coordinates

	aSourceOrigin.x = sourceSize.x * gridX;//coordinates.x;
	aSourceOrigin.y = sourceSize.y * gridY;//(coordinates.y);

	//testing
	//std::printf( "COORDS: x %d\n y %d\n", aSourceOrigin.x, aSourceOrigin.y );
	

	//send it in
	blit_masked_subregion(aSurface, *atlas, aSourceOrigin, sourceSize, aTranslation );
	//the angle seems to be wrong
	//not anymore hehehhehehehehe

}

SpaceShipAtlas make_spaceship_atlas()
{
	// TODO: Your implementation here
	auto image = load_image(SpaceShipAtlas::kSourceImage);
	
	return {std::move(image)};
}


// Sampler-based space ship
void SpaceShipSampler::draw( Surface& aSurface, float aAngleRadians, Vec2f aTranslation ) const
{
	// TODO: Your implementation here
	//there isa texture in spaceship sampler i think i need to use
	//auto ship = load_image(kSourceImage); //use auto so theres no errors
	Sampler2D sampler(*texture); //lol asterisk worked

	float image_width = texture->get_width();
	float image_height =  texture->get_height(); //poiunter so use ->

	//get 4 corners bbox
	Vec2f bot_left;
	bot_left.x = -(image_width/2.f);
	bot_left.y = -(image_height/2.f);
	Vec2f bot_right;
	bot_right.x = (image_width/2.f);
	bot_right.y = -(image_height/2.f);
	Vec2f top_left;
	top_left.x = -(image_width/2.f);
	top_left.y = (image_height/2.f);
	Vec2f top_right;
	top_right.x = (image_width/2.f);
	top_right.y = (image_height/2.f);

	//texture box
	Vec2f tex_bot_left;
	tex_bot_left.x = 0.f;
	tex_bot_left.y = 0.f;
	Vec2f tex_bot_right;
	tex_bot_right.x = image_width;
	tex_bot_right.y = 0.f;
	Vec2f tex_top_left;
	tex_top_left.x = 0.f;
	tex_top_left.y = image_height;
	Vec2f tex_top_right;
	tex_top_right.x = image_width;
	tex_top_right.y = image_height;

	Mat22f rotation = make_rotation_2d(aAngleRadians);
	bot_left = rotation * bot_left + aTranslation;
	bot_right = rotation * bot_right + aTranslation;
	top_left = rotation * top_left + aTranslation;
	top_right = rotation * top_right + aTranslation;

	draw_triangle_tex_masked(aSurface, bot_left, bot_right, top_right, sampler, tex_bot_left, tex_bot_right, tex_top_right);
	draw_triangle_tex_masked(aSurface, bot_left, top_right, top_left, sampler, tex_bot_left, tex_top_right, tex_top_left);

}

SpaceShipSampler make_spaceship_sampler()
{
	// TODO: Your implementation here
	//auto tex = std::make_unique<ImageRGBA>(load_image(SpaceShipSampler::kSourceImage));
	return SpaceShipSampler{load_image(SpaceShipSampler::kSourceImage)};
}

