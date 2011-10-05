# DrupalCONCEPT Pixel Access Counter

## Author

Markus Heurung <markus@freistil.it>  
Sponsored by: [freistil IT](http://freistil.it) for hosting at [DrupalCONCEPT](http://www.drupalconcept.com)

## Description

a little module, that inserts some Javascript to correct missing node access counter updates
when using a reverse proxy like Varnish.

Just put the module in sites/all/modules, activate it and you should be done.

It looks for anonymous sessions and does its work there. But it respects logged in users, where everything should go pass the cache and statistics should
normally.

## robots.txt

Put the following line into your robots.txt to prevent accidental access:

`Disallow: /ajax/dc_pixelaccess_stats.php`

## Credits

This module is based heavily on the [boost module](http://drupal.org/project/boost), 
espacially its boost_status.php script.