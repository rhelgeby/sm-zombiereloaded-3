/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volanticamp.inc
 *  Type:           Module
 *  Description:    Anti-camp handler.
 *
 *  Copyright (C) 2009  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

/**
 * Data structure for a anti-camp volume.
 */
enum VolTypeAnticamp
{
    Float:anticamp_interval,                    /** How often to trigger an action. */
    Handle:anticamp_timer,                      /** Action timer handle. */
    
    VolAnticampAction:anticamp_action,          /** What to do with players in anti-camp volumes */
    Float:anticamp_amount,                      /** Amount depending on action type. Usually time in seconds or damage amount. */
    
    VolAnticampeWarningType:anticamp_warning,   /** How to warn the player. */
    String:anticamp_message[256]                /** Override warning message. Max 256 characters. */    
}

/**
 * Actions to do with players in anti-camp volumes.
 */
enum VolAnticampAction
{
    anticamp_no_action,     /** Do nothing but give a warning. */
    anticamp_damage,        /** Give damage to player. */
    anticamp_slay,          /** Slay player. */
    anticamp_drug,          /** Drug player. */
    anticamp_fire           /** Ignite player. */
}

/**
 * Warning types.
 */
enum VolAnticampeWarningType
{
    anticamp_no_warning,    /** No warning messages. */
    anticamp_chat,          /** Print warning in chat area. */
    anticamp_center,        /** Print centered warning message. */
    anticamp_menu           /** Print a menu-like warning message with close option. */
}

/**
 * Anti-camp data.
 */
new AnticampData[ZR_VOLUMES_MAX][VolTypeAnticamp];
