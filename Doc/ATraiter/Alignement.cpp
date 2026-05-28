struct AlignmentOffsets {
    float offsetAz;
    float offsetAlt;
};

AlignmentOffsets alignTwoStars(
    float RA1, float DEC1,
    float RA2, float DEC2,
    float lat_deg, float LST_hours,
    float imuAz1, float imuAlt1,
    float imuAz2, float imuAlt2)
{
    float Az1_th, Alt1_th;
    float Az2_th, Alt2_th;

    // Position théorique des deux étoiles
    raDecToAzAlt(RA1, DEC1, lat_deg, LST_hours, Az1_th, Alt1_th);
    raDecToAzAlt(RA2, DEC2, lat_deg, LST_hours, Az2_th, Alt2_th);

    // Deltas
    float dAz1  = normalize180(imuAz1  - Az1_th);
    float dAlt1 = normalize180(imuAlt1 - Alt1_th);

    float dAz2  = normalize180(imuAz2  - Az2_th);
    float dAlt2 = normalize180(imuAlt2 - Alt2_th);

    // Offsets moyens
    AlignmentOffsets off;
    off.offsetAz  = (dAz1  + dAz2) / 2.0;
    off.offsetAlt = (dAlt1 + dAlt2) / 2.0;

    return off;
}
