const u16 *const gFormSpeciesIdTables[NUM_SPECIES] =
{
    [SPECIES_RATTATA] = sRattataFormSpeciesIdTable,
    [SPECIES_RATICATE] = sRaticateFormSpeciesIdTable,
    [SPECIES_RAICHU] = sRaichuFormSpeciesIdTable,
    [SPECIES_SANDSHREW] = sSandshrewFormSpeciesIdTable,
    [SPECIES_SANDSLASH] = sSandslashFormSpeciesIdTable,
    [SPECIES_VULPIX] = sVulpixFormSpeciesIdTable,
    [SPECIES_NINETALES] = sNinetalesFormSpeciesIdTable,
    [SPECIES_MEOWTH] = sMeowthFormSpeciesIdTable,
    [SPECIES_PERSIAN] = sPersianFormSpeciesIdTable,
    [SPECIES_GROWLITHE] = sGrowlitheFormSpeciesIdTable,
    [SPECIES_ARCANINE] = sArcanineFormSpeciesIdTable,
    [SPECIES_GEODUDE] = sGeodudeFormSpeciesIdTable,
    [SPECIES_GRAVELER] = sGravelerFormSpeciesIdTable,
    [SPECIES_GOLEM] = sGolemFormSpeciesIdTable,
    [SPECIES_PONYTA] = sPonytaFormSpeciesIdTable,
    [SPECIES_RAPIDASH] = sRapidashFormSpeciesIdTable,
    [SPECIES_VOLTORB] = sVoltorbFormSpeciesIdTable,
    [SPECIES_ELECTRODE] = sElectrodeFormSpeciesIdTable,
    [SPECIES_MAROWAK] = sMarowakFormSpeciesIdTable,
    [SPECIES_WEEZING] = sWeezingFormSpeciesIdTable,
    [SPECIES_TYPHLOSION] = sTyphlosionFormSpeciesIdTable,
    [SPECIES_SNEASEL] = sSneaselFormSpeciesIdTable,
    [SPECIES_ZIGZAGOON] = sZigzagoonFormSpeciesIdTable,
    [SPECIES_LINOONE] = sLinooneFormSpeciesIdTable,
    [SPECIES_CASTFORM] = sCastformFormSpeciesIdTable,
    [SPECIES_DEOXYS] = sDeoxysFormSpeciesIdTable,
#if P_GEN_4_POKEMON == TRUE
    [SPECIES_CHERRIM] = sCherrimFormSpeciesIdTable,
    [SPECIES_SHELLOS] = sShellosFormSpeciesIdTable,
    [SPECIES_GASTRODON] = sGastrodonFormSpeciesIdTable,
    [SPECIES_ROTOM] = sRotomFormSpeciesIdTable,
#endif
#if P_GEN_5_POKEMON == TRUE
    [SPECIES_SAMUROTT] = sSamurottFormSpeciesIdTable,
    [SPECIES_LILLIGANT] = sLilligantFormSpeciesIdTable,
    [SPECIES_DARUMAKA] = sDarumakaFormSpeciesIdTable,
    [SPECIES_DARMANITAN] = sDarmanitanFormSpeciesIdTable,
    [SPECIES_ZORUA] = sZoruaFormSpeciesIdTable,
    [SPECIES_ZOROARK] = sZoroarkFormSpeciesIdTable,
    [SPECIES_BRAVIARY] = sBraviaryFormSpeciesIdTable,
#endif
#if P_GEN_6_POKEMON == TRUE
    [SPECIES_AEGISLASH] = sAegislashFormSpeciesIdTable,
    [SPECIES_SLIGGOO] = sSliggooFormSpeciesIdTable,
    [SPECIES_GOODRA] = sGoodraFormSpeciesIdTable,
    [SPECIES_AVALUGG] = sAvaluggFormSpeciesIdTable,
#endif
#if P_GEN_7_POKEMON == TRUE
    [SPECIES_DECIDUEYE] = sDecidueyeFormSpeciesIdTable,
    [SPECIES_MIMIKYU] = sMimikyuFormSpeciesIdTable,
    [SPECIES_LYCANROC] = sLycanrocFormSpeciesIdTable,
#endif
#if P_GEN_8_POKEMON == TRUE
    [SPECIES_TOXTRICITY] = sToxtricityFormSpeciesIdTable,
    [SPECIES_EISCUE] = sEiscueFormSpeciesIdTable,
#endif
    // Alolan Forms
    [SPECIES_RATTATA_ALOLAN] = sRattataFormSpeciesIdTable,
    [SPECIES_RATICATE_ALOLAN] = sRaticateFormSpeciesIdTable,
    [SPECIES_RAICHU_ALOLAN] = sRaichuFormSpeciesIdTable,
    [SPECIES_SANDSHREW_ALOLAN] = sSandshrewFormSpeciesIdTable,
    [SPECIES_SANDSLASH_ALOLAN] = sSandslashFormSpeciesIdTable,
    [SPECIES_VULPIX_ALOLAN] = sVulpixFormSpeciesIdTable,
    [SPECIES_NINETALES_ALOLAN] = sNinetalesFormSpeciesIdTable,
    [SPECIES_MEOWTH_ALOLAN] = sMeowthFormSpeciesIdTable,
    [SPECIES_PERSIAN_ALOLAN] = sPersianFormSpeciesIdTable,
    [SPECIES_GEODUDE_ALOLAN] = sGeodudeFormSpeciesIdTable,
    [SPECIES_GRAVELER_ALOLAN] = sGravelerFormSpeciesIdTable,
    [SPECIES_GOLEM_ALOLAN] = sGolemFormSpeciesIdTable,
    [SPECIES_MAROWAK_ALOLAN] = sMarowakFormSpeciesIdTable,
    // Galarian Forms
    [SPECIES_PONYTA_GALARIAN] = sPonytaFormSpeciesIdTable,
    [SPECIES_RAPIDASH_GALARIAN] = sRapidashFormSpeciesIdTable,
    [SPECIES_WEEZING_GALARIAN] = sWeezingFormSpeciesIdTable,
    [SPECIES_ZIGZAGOON_GALARIAN] = sZigzagoonFormSpeciesIdTable,
    [SPECIES_LINOONE_GALARIAN] = sLinooneFormSpeciesIdTable,
#if P_GEN_5_POKEMON == TRUE
    [SPECIES_DARUMAKA_GALARIAN] = sDarumakaFormSpeciesIdTable,
    [SPECIES_DARMANITAN_GALARIAN] = sDarmanitanFormSpeciesIdTable,
#endif
    //Hisuian Forms
    [SPECIES_GROWLITHE_HISUIAN] = sGrowlitheFormSpeciesIdTable,
    [SPECIES_ARCANINE_HISUIAN] = sArcanineFormSpeciesIdTable,
    [SPECIES_VOLTORB_HISUIAN] = sVoltorbFormSpeciesIdTable,
    [SPECIES_ELECTRODE_HISUIAN] = sElectrodeFormSpeciesIdTable,
    [SPECIES_TYPHLOSION_HISUIAN] = sTyphlosionFormSpeciesIdTable,
    [SPECIES_SNEASEL_HISUIAN] = sSneaselFormSpeciesIdTable,
#if P_GEN_5_POKEMON == TRUE
    [SPECIES_SAMUROTT_HISUIAN] = sSamurottFormSpeciesIdTable,
    [SPECIES_LILLIGANT_HISUIAN] = sLilligantFormSpeciesIdTable,
    [SPECIES_ZORUA_HISUIAN] = sZoruaFormSpeciesIdTable,
    [SPECIES_ZOROARK_HISUIAN] = sZoroarkFormSpeciesIdTable,
    [SPECIES_BRAVIARY_HISUIAN] = sBraviaryFormSpeciesIdTable,
#endif
#if P_GEN_6_POKEMON == TRUE
    [SPECIES_SLIGGOO_HISUIAN] = sSliggooFormSpeciesIdTable,
    [SPECIES_GOODRA_HISUIAN] = sGoodraFormSpeciesIdTable,
    [SPECIES_AVALUGG_HISUIAN] = sAvaluggFormSpeciesIdTable,
#endif
#if P_GEN_7_POKEMON == TRUE
    [SPECIES_DECIDUEYE_HISUIAN] = sDecidueyeFormSpeciesIdTable,
#endif
    // Castform
    [SPECIES_CASTFORM_SUNNY] = sCastformFormSpeciesIdTable,
    [SPECIES_CASTFORM_RAINY] = sCastformFormSpeciesIdTable,
    [SPECIES_CASTFORM_SNOWY] = sCastformFormSpeciesIdTable,
    // Deoxys
    [SPECIES_DEOXYS_ATTACK] = sDeoxysFormSpeciesIdTable,
    [SPECIES_DEOXYS_DEFENSE] = sDeoxysFormSpeciesIdTable,
    [SPECIES_DEOXYS_SPEED] = sDeoxysFormSpeciesIdTable,
#if P_GEN_4_POKEMON == TRUE
    // Cherrim
    [SPECIES_CHERRIM_SUNSHINE] = sCherrimFormSpeciesIdTable,
    // Shellos
    [SPECIES_SHELLOS_EAST_SEA] = sShellosFormSpeciesIdTable,
    // Gastrodon
    [SPECIES_GASTRODON_EAST_SEA] = sGastrodonFormSpeciesIdTable,
    // Rotom
    [SPECIES_ROTOM_HEAT] = sRotomFormSpeciesIdTable,
    [SPECIES_ROTOM_WASH] = sRotomFormSpeciesIdTable,
    [SPECIES_ROTOM_FROST] = sRotomFormSpeciesIdTable,
    [SPECIES_ROTOM_FAN] = sRotomFormSpeciesIdTable,
    [SPECIES_ROTOM_MOW] = sRotomFormSpeciesIdTable,
#endif
#if P_GEN_5_POKEMON == TRUE
    // Darmanitan
    [SPECIES_DARMANITAN_ZEN_MODE] = sDarmanitanFormSpeciesIdTable,
    [SPECIES_DARMANITAN_ZEN_MODE_GALARIAN] = sDarmanitanFormSpeciesIdTable,
#endif
#if P_GEN_6_POKEMON == TRUE
    // Aegislash
    [SPECIES_AEGISLASH_BLADE] = sAegislashFormSpeciesIdTable,
#endif
#if P_GEN_7_POKEMON == TRUE
    // Lycanroc
    [SPECIES_LYCANROC_MIDNIGHT] = sLycanrocFormSpeciesIdTable,
    [SPECIES_LYCANROC_DUSK] = sLycanrocFormSpeciesIdTable,
    // Mimikyu
    [SPECIES_MIMIKYU_BUSTED] = sMimikyuFormSpeciesIdTable,
#endif
#if P_GEN_8_POKEMON == TRUE
    // Toxtricity
    [SPECIES_TOXTRICITY_LOW_KEY] = sToxtricityFormSpeciesIdTable,
    // Eiscue
    [SPECIES_EISCUE_NOICE_FACE] = sEiscueFormSpeciesIdTable,
#endif
};
