/** \file StashData.cpp
	Source that defines objects that stash cooked parameter data.

 * Cop2 Nodes rely on highly-threaded calls to calculate the image pixels.
 * As a result, Cops has a builting concept of a ContextData object that
 * is meant to hold onto the parameters, and is copied to each thread so that
 * parameters aren't cooked dozens of times. CCFS takes this one step further,
 * by using these StashData objects as though they were threadsafe 'parameter'
 * getters. This was necessary, because of how many different parms are shared
 * by nodes in the CCFS.
 */

 // Local
#include "StashData.h"
#include "typedefs.h"
#include "FractalSpace.h"

CC::XformStashData::XformStashData(
	fpreal offset_x, fpreal offset_y,
	fpreal rotate, fpreal scale, RSTORDER xord) :
	offset_x(offset_x), offset_y(offset_y),
	rotate(rotate), scale(scale), xord(xord) {}

void
CC::XformStashData::evalArgs(const OP_Node* node, fpreal t)
{
	scale = node->evalFloat(SCALE_NAME.first, 0, t);
	offset_x = node->evalFloat(TRANSLATE_NAME.first, 0, t);
	offset_y = node->evalFloat(TRANSLATE_NAME.first, 1, t);
	rotate = node->evalFloat(ROTATE_NAME.first, 0, t);

	/// Make xord not necessary, mainly to play nice with
	/// TEMPLATES_XFORM_BUDDHABROT which has it removed
	if (node->hasParm(XORD_NAME.first))
		xord = get_rst_order(node->evalInt(XORD_NAME.first, 0, t));
	else
		xord = RSTORDER::STR;
}

CC::MandelbrotStashData::MandelbrotStashData(
	int iters, fpreal power, fpreal bailout,
	int jdepth, COMPLEX joffset,
	bool blackhole) :
	iters(iters), power(power), bailout(bailout),
	jdepth(jdepth),
	joffset(joffset),
	blackhole(blackhole)
{}

void
CC::MandelbrotStashData::evalArgs(const OP_Node * node, fpreal t)
{
	iters = node->evalInt(ITERS_NAME.first, 0, t);
	power = node->evalFloat(POWER_NAME.first, 0, t);
	bailout = node->evalFloat(BAILOUT_NAME.first, 0, t);
	jdepth = node->evalInt(JDEPTH_NAME.first, 0, t);
	fpreal joffset_x = node->evalFloat(JOFFSET_NAME.first, 0, t);
	fpreal joffset_y = node->evalFloat(JOFFSET_NAME.first, 1, t);
	joffset = COMPLEX(joffset_x, joffset_y);
	int rawblackhole = node->evalInt(BLACKHOLE_NAME.first, 0, t);
	blackhole = rawblackhole > 0; // Make boolean
}

CC::PickoverStashData::PickoverStashData(
	int iters, fpreal power, fpreal bailout,
	int jdepth, COMPLEX joffset, bool blackhole,
	COMPLEX popoint, fpreal porotate, bool pomode,
	bool poref, fpreal porefsize) :
	MandelbrotStashData(iters, power, bailout, jdepth, joffset, blackhole),
	popoint(popoint), porotate(porotate), pomode(pomode),
	poref(poref), porefsize(porefsize)
{}

void
CC::PickoverStashData::evalArgs(const OP_Node * node, fpreal t)
{
	/// Call the mandelbrot stash values first.
	CC::MandelbrotStashData::evalArgs(node, t);

	/// Call pickover-specific methods second.
	fpreal popoint_x = node->evalFloat(POPOINT_NAME.first, 0, t);
	fpreal popoint_y = node->evalFloat(POPOINT_NAME.first, 1, t);
	popoint = COMPLEX(popoint_x, popoint_y);
	porotate = node->evalFloat(POROTATE_NAME.first, 0, t);
	pomode = node->evalInt(POMODE_NAME.first, 0, t);
	poref = node->evalInt(POREFERENCE_NAME.first, 0, t);
	porefsize = node->evalFloat(POREFSIZE_NAME.first, 0, t);
}

void
CC::LyapunovStashData::evalArgs(const OP_Node * node, fpreal t)
{
	iters = node->evalFloat(ITERS_NAME.first, 0, t);
	start = node->evalFloat(LYASTART_NAME.first, 0, t);
	maxval = node->evalInt(LYACEILVALUE_NAME.first, 0, t);
	invertnegative = node->evalInt(LYAINVERTNEGATIVE_NAME.first, 0, t);

	// Multiparm load attribs
	int seqSize = node->evalInt(LYASEQ_NAME.first, 0, t);
	seq.reserve(seqSize);

	for (int i = 1; i <= seqSize; ++i)
	{
		seq.emplace_back(
			node->evalFloatInst(LYASEQWEIGHTS_NAME.first, &i, 0, t));
	}
}

CC::LyapunovStashData::~LyapunovStashData()
{
}

void
CC::MultiXformStashData::evalArgs(
	const OP_Node * node, fpreal t)
{
	int numXforms = node->evalInt(XFORMS_NAME.first, 0, t);
	xforms.reserve(numXforms);

	fpreal offset_x, offset_y, rotate, scale;
	offset_x = offset_y = rotate = scale = 0;

	RSTORDER xord{ RSTORDER::RST };

	// Create a XformStashData object for each # in the multiparm.
	for (int i = 1; i <= numXforms; ++i)
	{
		offset_x = node->evalFloatInst(TRANSLATE_M_NAME.first, &i, 0, t);
		offset_y = node->evalFloatInst(TRANSLATE_M_NAME.first, &i, 1, t);
		rotate = node->evalFloatInst(ROTATE_M_NAME.first, &i, 0, t);
		scale = node->evalFloatInst(SCALE_M_NAME.first, &i, 0, t);
		xord = static_cast<RSTORDER>(
			node->evalIntInst(XORD_M_NAME.first, &i, 0, t));

		xforms.emplace_back(XformStashData(
			offset_x, offset_y, rotate, scale, xord));
	}
}
