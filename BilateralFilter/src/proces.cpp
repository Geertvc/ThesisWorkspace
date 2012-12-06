/*
 * proces.cpp
 *
 *  Created on: Nov 12, 2012
 *      Author: thypo
 */

#include "proces.hpp"
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>

//#define BOOST_GIL_USE_CONCEPT_CHECK 1


template <typename SrcView, typename DstView>
void process(const SrcView& src, const DstView& dst) {
	using namespace boost::gil;
	gil_function_requires<ImageViewConcept<SrcView> >();
    gil_function_requires<MutableImageViewConcept<DstView> >();
    gil_function_requires<ColorSpacesCompatibleConcept<
                                typename color_space_type<SrcView>::type,
                                typename color_space_type<DstView>::type> >();

    // compute the gradient
}
