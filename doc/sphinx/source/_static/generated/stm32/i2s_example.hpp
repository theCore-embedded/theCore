/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Header provides template of SPI and I2S drivers configuration.
#ifndef STM32XX_SPI_I2S_CFGS_HPP_
#define STM32XX_SPI_I2S_CFGS_HPP_

#include <aux/platform_defines.hpp>
#include <aux/spi_bus.hpp>

namespace ecl
{


// SPI configuration -----------------------------------------------------------

// TODO

// I2S configuration -----------------------------------------------------------


template<>
struct spi_i2s_cfg<spi_device::bus3>
{
    static constexpr auto bus_type = spi_bus_type::i2s;

    using dma_tx = i2s3_tx_dma;
    using dma_rx = i2s3_tx_dma;

    static constexpr I2S_InitTypeDef init_obj = {
        I2S_Mode_MasterTx,
        I2S_Standard_Phillips,
        I2S_DataFormat_16b,
        I2S_MCLKOutput_Enable,
        I2S_AudioFreq_8k,
        I2S_CPOL_High,
    };
};


using I2S3_driver = spi_i2s_bus<spi_device::bus3>;
using the_i2s3_alias = I2S3_driver;

} // namespace ecl

#endif // STM32XX_SPI_I2S_CFGS_HPP_


