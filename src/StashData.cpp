/*
	Cole Clark's Fractal Suite

	StashData.cpp
	Code implementing stashed data.
 */

#include "StashData.h"
#include "typedefs.h"
#include "FractalSpace.h"

using namespace CC;

XformStashData::XformStashData(
	double offset_x, double offset_y,
	double rotate, double scale, RSTORDER xord) : 
		offset_x(offset_x), offset_y(offset_y),
		rotate(rotate), scale(scale), xord(xord) {}

void XformStashData::evalArgs(const OP_Node* node, fpreal t)
{
	scale = node->evalFloat(SCALE_NAME, 0, t);
	offset_x = node->evalFloat(TRANSLATE_NAME, 0, t);
	offset_y = node->evalFloat(TRANSLATE_NAME, 1, t);
	rotate = node->evalFloat(ROTATE_NAME, 0, t);
	xord = get_rst_order(node->evalInt(XORD_NAME, 0, t));
}