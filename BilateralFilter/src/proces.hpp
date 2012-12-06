/*
 * proces.hpp
 *
 *  Created on: Nov 12, 2012
 *      Author: thypo
 */

#ifndef PROCES_HPP_
#define PROCES_HPP_


#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>

template <typename SrcView, typename DstView>
void process(const SrcView& src, const DstView& dst);


#endif /* PROCES_HPP_ */
