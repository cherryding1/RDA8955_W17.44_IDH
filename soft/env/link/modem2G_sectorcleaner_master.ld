

/* Take into account assert bug in binutils-2.15*/
#if (USE_BINUTILS_2_19 == 1)
#define LD_ASSERT(condition, small, full)   ASSERT(condition, full)
#else
#define LD_ASSERT(condition, small, full)   ASSERT(condition, small)
#endif


SECTIONS
{
    /*================================================= */
    /* Define user data boundaries based on the defines */
    /*================================================= */
    _user_data_start = USER_DATA_BASE;
    _user_data_end = USER_DATA_BASE+USER_DATA_SIZE;
    _user_data_size = USER_DATA_SIZE;

#if (CLEAN_USER_SECTOR == 1)
    .user_data USER_DATA_BASE : AT(ADDR(.user_data))
    {
        LONG(0xffffffff)
        . = USER_DATA_SIZE;
    } = 0xffffffff
#endif /* (CLEAN_USER_SECTOR == 1) */


    /* ============================== */
    /* calib sector in flash          */
    /* ============================== */

    /*Check that the user data in flash do not overwrite
     * the calib data space */
    LD_ASSERT(ABSOLUTE(CALIB_BASE) >= ABSOLUTE(_user_data_end), "CALO", "Calib data is overlapped with user data.")

#if (CLEAN_CALIB_SECTOR == 1)
    .calib CALIB_BASE : AT(ADDR(.calib))
    {
        _calib_start = .;
        *(.calib);
        LONG(0xffffffff)
        . = MIN( (FLASH_BASE-0xffffffff00000000) + FLASH_SIZE - (CALIB_BASE-0xffffffff00000000),
                 (FACT_SETTINGS_BASE-0xffffffff00000000) - (CALIB_BASE-0xffffffff00000000) );
    } = 0xffffffff
    _calib_end = .;
    _calib_size = _calib_end - _calib_start;
#endif /* (CLEAN_CALIB_SECTOR == 1) */


    /* ============================== */
    /* factory settings in flash      */
    /* ============================== */

    /*Check that the user data in flash do not overwrite
     * the factory setting data space */
    LD_ASSERT(ABSOLUTE(FACT_SETTINGS_BASE) >= ABSOLUTE(_user_data_end), "FACO", "Factory setting data is overlapped with user data.")

#if (CLEAN_FACT_SECTOR == 1)
    .factory_settings FACT_SETTINGS_BASE : AT(ADDR(.factory_settings))
    {
        _factory_start = .;
        *(.factory_settings)
        LONG(0xffffffff)
        . = MIN( (FLASH_BASE-0xffffffff00000000) + FLASH_SIZE - (FACT_SETTINGS_BASE-0xffffffff00000000),
                 (CALIB_BASE-0xffffffff00000000) - (FACT_SETTINGS_BASE-0xffffffff00000000) );
    } = 0xffffffff
    _factory_end = .;
    _factory_size = _factory_end - _factory_start;
#endif /* (CLEAN_FACT_SECTOR == 1) */


    /DISCARD/ :
    {
        *(*)
    }
}

