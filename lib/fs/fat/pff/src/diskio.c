/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/

#include "diskio.h"


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (DISK_TYPE* disk) /* Disk descriptor */
{
    DSTATUS stat = disk->disk_initialize(disk->disk_obj);
    return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
        DISK_TYPE* disk,/* Disk descriptor */
        BYTE* buff,		/* Pointer to the destination object */
        DWORD sector,	/* Sector number (LBA) */
        UINT offset,	/* Offset in the sector */
        UINT count		/* Byte count (bit15:destination) */
        )
{
    DRESULT res = disk->disk_readp(disk->disk_obj,
                                   buff,
                                   sector,
                                   offset,
                                   count);

    return res;
}



/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

DRESULT disk_writep (
        DISK_TYPE*  disk,    /* Disk descriptor */
        const BYTE* buff,	/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
        DWORD sc			/* Sector number (LBA) or Number of bytes to send */
        )
{
    DRESULT res = 0;

    (void) disk;

    if (!buff) {
        if (sc) {

            // Initiate write process

        } else {

            // Finalize write process

        }
    } else {

        // Send data to the disk

    }

    return res;
}

