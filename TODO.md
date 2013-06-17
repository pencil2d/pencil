Pencil TODO List
=================================

Last edited: 2013-06-18
Pascal Naidon, chchwy


Bugs - things to do
-------------------

### BUGS:

* bug: memory leaks??
* bug when closing a curve -> straight line or even nothing
* bug selected point moved --->
* bug: strange long things protruding out

* bug when variable width and zoom - FIXED
* bug when adding point -> area shrinks - FIXED
* bug : new file --> bus error (FIXED?)
* bug : when deleting/adding layers - FIXED
* bug : when drawing 2nd vec layer - FIXED
* bug : when moving a bitmap selection with transparent colour (FIXED?)



### THINGS TO IMPLEMENT:

* compress the Pencil files (XML + data (images)) into a single ZIP archive - similarly to Open Office Documents.
  implement a open and save function for these ZIP archives

* improve movie export (get a QuickTime dialog, AVI on Windows <DONE>, MPEG...)

* fast gaussian blur for smooth shading




### SUGGESTIONS FROM USERS:

- duplicate frames - DONE
- rotate - DONE ---> but bug when drawing upside down and very slow
- free selection tool
- animate selection, and disable selection
- save colour palettes
- shortcuts in the menus - DONE
- customise hotkeys
- import a sequence of images - DONE (drag and drop only)


### OTHER IDEAS:

* operate on several (selected?) images at a time
* morphing/tweening...?
* multiple documents



### THINGS TO IMPROVE:

- Flood fill:
    * problem if a corner is too sharp ---> increase tolerance? temporarily change width? DONE (but for very sharp? DONE! (up to 3 points) )
    * problem: sometimes the final leaves are not next to each other - OBSOLETE
    * reduce contour pixels to pixels which are in the rectangular area - DONE
    * improve intersections!
    * test if the initial point is contained in the area - DONE
    * update areas when adding a point - DONE
    * remove points
    * bug when remove colour - DONE


* manage colours
----> "transparent" colour; for shade contour
interactive change of colour - DONE???

* write/read vectorial data - DONE

* better eraser vectorial
* several layers - DONE
* eraser tool for vectorial - DONE
* separate bitmap / vectorial - DONE
* improve timeline management - DONE
* warn when using a tool on hidden layer - DONE

* improve the "cancel" function - DONE

* blur tool, spray
* improve simplification/inerpolation of curves

* copy/paste - DONE
* import sound - DONE
* reestablish export movie - DONE
* mirror option - flip horizontal - DONE
* double-click on hand to re-establish - DONE
* implement paint bucket, polyline for bitmap - DONE
* preferences - DONE

* accelerate : when moving the mouse, cache the background and draw only the modified element - DONE (but memory leaks?)
* change structure??? -> intersections re-calculated dynamically?

* use the OS clipboard for pasting - DONE
* CREATE a class "Image" --> BITMAP, VECTORIAL... - DONE


* add attribute for thin lines - DONE
* add attribute for transparency - DONE
* implement eyedropper - DONE


* improve selection of one curve
* improve selection of frames in timeline - DONE

* suppress areas - DONE



============= WIN XP ============
implement export movie - DONE
implement export flash - DONE
flash compression

bug grey border - FIXED
bug export jpeg - FIXED
bug file extension - FIXED

bug release - FIXED
bug with tablet - CIRCUMVENTED with option
bug - tablet doesn't work??

