/*
    Copyright (c) 2023 Ulf Lorenz

    Tensor is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published
    by the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "loops.h"
#include <gtest/gtest.h>
#include <tensor/tensor.h>

namespace tensor_test {

TEST(SlicingUsageTest, SliceMultiplicationAndAssignment)
{
    RTensor input{{1.0, 2.0}, {3.0, 4.0}};

    RTensor output = RTensor::empty(input.dimensions());
    output.at(_, range(0,0)) = 2.0 * input(_, range(0,0));
    output.at(_, range(1,1)) = 3.0 * input.at(_, range(1,1));

    RTensor expected{{2.0, 6.0}, {6.0, 12.0}};
    EXPECT_ALL_EQUAL(expected, output);
}

} // namespace tensor_test