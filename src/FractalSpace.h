/*
	Cole Clark's Fractal Suite

	FractalSpace.h
	Header defining everything related to Xforms in Fractal Space.
 */

#pragma once

#include <TIL/TIL_Tile.h>
#include <TIL/TIL_TileList.h>
#include <UT/UT_Matrix3.h>
#include <UT/UT_XformOrder.h>

#include <utility>

typedef std::pair<double, double> FCOORDS;
typedef std::pair<int, int> WORLDPIXELCOORDS;
typedef UT_XformOrder::rstOrder RSTORDER;

namespace CC
{
	///Get the Houdini rstOrder enum value from the interface
	RSTORDER get_rst_order(const int val);

	WORLDPIXELCOORDS calculate_world_pixel(
		TIL_TileList* tiles,
		TIL_Tile* tile,
		int pixel_index);


	class FractalSpace
	{
		int image_x{ 0 };
		int image_y{ 0 };
		UT_Matrix3 post_matrix;
		RSTORDER rstorder{ RSTORDER::TRS };

	public:

		FractalSpace();

		void set_xform(
			const double tx,
			const double ty,
			const double r,
			const double sx,
			const double sy,
			const double r_pivx,
			const double r_pivy,
			const double s_pivx,
			const double s_pivy,
			const RSTORDER xord);

		FCOORDS get_fractal_coords(WORLDPIXELCOORDS pixel_coords);

		void set_image_size(int x, int y);
		WORLDPIXELCOORDS get_image_size();

		virtual ~FractalSpace();

	};
}