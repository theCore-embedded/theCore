/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Header provides template of SPI and spi drivers configuration.
#ifndef PLATFORM_SPI_CFGS_HPP_
#define PLATFORM_SPI_CFGS_HPP_

#include <aux/platform_defines.hpp>
#include <aux/spi.hpp>

namespace ecl
{

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup tm4c Texas Instruments Tiva C TM4C123G platform
//! @{

//! \defgroup tm4c_templates Auto-generated code, Python COG and CMake templates.
//! @{


// SPI configuration -----------------------------------------------------------

/* SPI1 instance */

template<>
struct spi_cfg<spi_channel::ch1>
{
    static constexpr auto type = ecl::spi_type::master;
    static constexpr auto cpol = ecl::spi_cpol::low;
    static constexpr auto cpha = ecl::spi_cpha::low;
};


using spi_1_driver = spi<spi_channel::ch1>;
/* SPI3 instance */

template<>
struct spi_cfg<spi_channel::ch3>
{
    static constexpr auto type = ecl::spi_type::master;
    static constexpr auto cpol = ecl::spi_cpol::low;
    static constexpr auto cpha = ecl::spi_cpha::high;
};


using spi_3_driver = spi<spi_channel::ch3>;
using test_spi = spi_3_driver;

//! @}

//! @}

//! @}

} // namespace ecl

#endif // PLATFORM_SPI_CFGS_HPP_
