/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          volanticamp.inc
 *   Type:          Module
 *   Description:   Anti-camp handler.
 *
 * ============================================================================
 */

/**
 * Data structure for a anti-camp volume.
 */
enum VolTypeAnticamp
{
    anticamp_damage,
    anticamp_interval,
    Handle:anticamp_timer
}

/**
 * Anti-camp data.
 */
new AnticampData[ZR_VOLUMES_MAX][VolTypeAnticamp];
