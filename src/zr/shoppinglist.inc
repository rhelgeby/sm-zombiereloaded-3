/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          shoppinglist.inc
 *  Type:          Core
 *  Description:   Module to handle shopping list style lists.
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
 * This file is meant for formatting a given string into a list formatted around the style of a shopping
 * list.
 * Example:
 * String: "Bacon, Egg, Egg, Milk, Butter"
 * Shopping List: "Bacon, Eggx2, Milk, Butter"
 * Function parameters will include options to change the "," to any character.  As well as the "x2" format.
 * 
 * More complex examples:
 * String: "Red - Green - Blue-Red- Green -Green - Yellow -Grey"  Note: Horribly formatted, but we can handle it.
 * Shopping List: "Red(2)\nGreen(3)\nBlue\nYellow\nGrey" Note: We can change what to separate each item with, as well as how to display the quantity.
 * 
 * List Options:
 * 
 * Parse: Take a string and dump out a list.  Simple.
 * Set: Easy to do without this API, but maybe we can simplify it a little.
 * Add: Should be a simple format.  May be in the API just to simplify it a bit. 
 * Remove: Removes an item from the list.  Must remove from the end to preserve list order.
 *  
 * Scenario:
 * String: "Apple, Orange, Mango, Orange"
 * Shopping List: "Apple, Orange x2, Mango"
 * Remove: "Orange"
 * Remove first orange: "Apple, Mango, Orange"  Different order.
 * Remove last orange: "Apple, Orange, Mango" Yay.
 * 
 */

/**
 * Takes a given string and outputs a re-formatted shopping list styled list.  See description above.
 * Note:  This is for display purposes only, meaning there will be no API made to format the output of this function.
 * If you plan on the list being dynamic, store the raw string and use the API to edit those.
 * 
 * @param list              Raw string to be formatted.
 * @param list_maxlen       The maximum length of the raw string.
 * @param shoppinglist      Outputted shopping list.
 * @param slist_maxlen      The maximum length of the shopping list.
 * @param in_token          The token used to separate each item in the raw string. Ex: "Apple, Orange"  Token: ","
 * @param out_token         What to separate each item with in the shopping list.  Ex: Token: "\n"  List: "Apple\nOrange"
 * @param quantityformat    How to show the quantity of an item for multi-listed items.  %d is the number.  Ex: "Apples x2" quantityformat: " x%d"
 */
stock ShoppingListFormat(const String:list[], list_maxlen, String:shoppinglist[], slist_maxlen, const String:in_token[] = ",", const String:out_token[] = ",", const String:quantityformat[] = " x%d")
{
}

/**
 * Set the list to a given string array of items.
 * 
 * @param list      The variable to store new list in.
 * @param maxlen    The maximum length of the finished list.
 * @param items     The items to construct into the list.
 * @param maxitems  The number of array indexes from 0 to process for the list.
 * @param token     The token to use to append the item.  Set to "" to auto-detect an existing token, if
 *                  there is only one item in the list, this parameter won't be used.
 */
stock ShoppingListConstruct(String:list[], maxlen, const String:items[][], maxitems, const String:token)
{
}

/**
 * Add an item to the list.
 * 
 * @param list      The list to add item to.
 * @param maxlen    The maximum length of the finished list.
 * @param item      The item to add to the list.
 * @param token     The token to use to append the item.  Set to "" to auto-detect an existing token, if
 *                  this is the first item in the list, this parameter won't be used.
 */
stock ShoppingListAppend(String:list[], maxlen, const String:item[], const String:token)
{
}

/**
 * Add an item to the list.
 * 
 * @param list      The list to remove item from.
 * @param maxlen    The maximum length of the finished list.
 * @param item      The item to remove from the list.
 */
stock ShoppingListRemove(String:list[], maxlen, const String:item[])
{
}