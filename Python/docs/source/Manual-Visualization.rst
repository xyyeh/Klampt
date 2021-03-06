Visualization
=============================

Klamp't allows you to easily produce custom interactive visualizations
with just a few lines of Python code.  This is extremely debug

The `klampt.vis <klampt.vis.html>`__ module supports two frameworks for
doing so:

-  Method 1: use the builtin *scene manager*.
-  Method 2: overload
   :class:`~klampt.vis.glinterface.GLPluginInterface`
   and customize the event handling and drawing routines.

Method 1 is easier to set up, and is more intuitive for users who may be
unfamiliar with the event-driven paradigm used in GUI programming.
However, Method 2 is necessary to get control over the low-level GUI
event loop. They can also be hybridized to customize how the scene
manager responds to user input.

Examples of these methods in action are available in the
``Klampt-examples/Python/demos/vis_template.py`` script.

.. note::
    `klampt.vis` is not imported when you run `import klampt`, because some
    systems don't have the capability to run OpenGL.  You will need to run
    `import klampt.vis` or `from klampt import vis`.

Visualization window management
--------------------------------

Window creation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When ``klampt.vis`` is imported, it can be thought of as
having an existing hidden window which hosts the scene manager. You will
then configure the window or scene manager, and in the most cross-platform
compatible mode of operation, you will show it using one
of the following methods.

-  ``vis.spin(duration)``: shows the window until it is closed or
   ``duration`` seconds have elapsed.
-  ``vis.dialog()``: shows the current window in a dialog format, and does
   not return until the user closes the window or presses the OK button.
-  ``vis.run()``: shows the window, and once the user closes the window,
   the visualization is killed.
-  ``vis.kill()``: performs all cleanup of the vis module.

These methods block the calling thread until the window is closed.
You can call ``spin`` and ``dialog`` multiple times in a row.  If you have PyQt
installed, and want to customize the UI, you can use the method

-  ``vis.customUI(makefunc)``: takes a 1-parameter function
   makefunc(glwidget) that takes the Klampt QGLWidget as an argument,
   creates a QWindow or QDialog to be shown, and returns it.

Multithreaded mode
~~~~~~~~~~~~~~~~~~~

In Linux and Windows, a *multithreaded mode* is available which allows you
to conveniently run visualizations in parallel with your main code.  This
means you can very easily pop up a visualization window to observe a
processing loop (such as a simulation or planner) in real-time.

Suppose you had a loop like this:

.. code:: python

    import klampt
    
    world = klampt.WorldModel()
    #...configure stuff...
    while not done():
      #...do stuff to world... 

The corresponding live visualization of the loop would look like this:

.. code:: python

    import klampt
    from klampt import vis

    world = klampt.WorldModel()
    vis.add("world",world)    #world is now referenced by the vis module and is shared between threads!
    #...configure stuff...
    vis.show()
    while not done() and vis.shown():
      vis.lock()
      #...do stuff to world... #this code is executed at approximately 10 Hz due to the sleep call
      vis.unlock()
      time.sleep(0.1)
    if done():
      vis.show(False)         #hides the window if not closed by user

Specifically, the multithreaded mode uses the following functions:

-  ``vis.show()``: shows the current window and returns immediately to
   the calling thread.
-  ``vis.shown()``: returns True if the window is shown and not closed
   by the user.
-  ``vis.show(False)``: hides the current window.

   .. note::

      ``vis.hide()`` doesn't do the opposite of ``vis.show``.  It refers to
      hiding items in the scene manager.

When you call ``show`` the visualization is run in a separate thread from
the main Python script. The visualization and Klamp't objects that it refers to can then be
configured and modified by the main Python thread. However, some care is
needed when directly modifying Klamp't objects that are referred to in
the visualization. To prevent conflicts in threading which may cause the
program to crash, all references to shared objects in the main thread
should be placed between ``vis.lock()`` and ``vis.unlock()`` calls, as shown
in the above code.

Multithreaded mode workaround on Mac
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For Mac users, multithreaded mode is not available.  You can mimic a
multithreaded loop using the ``vis.loop()`` function, which takes several
callback functions to be run inside the visualization loop.  This version
is written as follows:

.. code:: python

    world = klampt.WorldModel()
    #...configure stuff...

    vis.add("world",world)

    def setup():
      vis.show()

    def callback():
      #...do stuff to world... #this code is executed at approximately 10 Hz due to the sleep call
      time.sleep(0.1)
      if done():
        vis.show(False)         #hides the window if not closed by user

    def cleanup():
      #can perform optional cleanup code here
      pass

    vis.loop(setup=setup,callback=callback,cleanup=cleanup)

Note that the ``loop`` function can also be run on Linux and Windows, so
if you are writing cross-platform code, the main rule to remember is not to use
``vis.show()`` outside of a loop setup callback.



The plugin stack
~~~~~~~~~~~~~~~~~

The vis module lets you *override* or *stack* plugins together,
even with the existing scene graph manager. In fact, the scene graph
manager is itself a plugin. 

Each window has a *plugin stack* with at least one plugin.
The stack can be modified using the following functions:

-  ``vis.setPlugin`` overrides the plugin stack used by the current
   window.
-  ``vis.pushPlugin`` and ``vis.popPlugin`` modify the plugin stack used
   by the current window.


Split screen and multiple windows
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To do split screen, call ``vis.addPlugin(plugin)`` with the root plugin
for the new viewport.

The vis module can handle multiple windows. The vis module stores an
*active window*, which is the window to which subsequent vis calls will
be passed. (not the window currently selected by the user). The relevant
functions are:

-  ``vis.createWindow()``: creates and returns the identifier for a new
   window. If this is the first createWindow call, no new window is
   created, and instead the ID of the hidden window is returned.
-  ``vis.setWindow(id)``: changes the active window.
-  ``vis.getWindow()``: returns the active window.




klampt.vis Scene Manager
-------------------------

Using the scene manager, the main thread can easily add and remove items
to be drawn. Simple functions are available to build multi-viewport
GUIs, to customize appearances, control animations, and other
visualization functions. For more information see the documentation of
`klampt.vis.visualization <klampt.vis.html>`__,
and the example code in
``Klampt-examples/Python/demos/vistemplate.py``.

-  ``vis.add(name,item)``: adds a named item to the scene manager.
-  ``vis.clear()``: clears all items.
-  ``vis.remove(name)``: removes an existing item.
-  ``vis.hide(name,hidden=True)``: hides/unhides an existing item.

   .. note::
      ``vis.show()`` doesn't do the opposite of ``vis.hide()``.  To
      show an item, call ``vis.hide(False)``.

-  ``vis.edit(name,doedit=True)``: turns on/off visual editing, if the
   item allows it.

Here are the accepted types in the scene manager.

+-----------------------------+------------------------------------------+------------------------------------------+
|    Type                     | Notes                                    | Attributes                               |
+=============================+==========================================+==========================================+
| ``str``                     | Draws a label                            | ``position``\*                           |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``WorldModel``              |                                          |                                          |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``RobotModel``              |                                          |                                          |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``RigidObjectModel``        |                                          |                                          |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``TerrainModel``            |                                          |                                          |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``Geometry3D``              |                                          |                                          |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``Vector3``                 |                                          | ``size`` (5)                             |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``RigidTransform``          |                                          | ``fancy`` (False), ``length`` (0.1),     |
|                             |                                          | ``width`` (0.01)                         |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``Config``                  | Shows a ghost of the robot               |                                          |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``Configs``                 |                                          | ``maxConfigs`` (20)                      |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``Trajectory``              | Draws 3D, SE(3), or end-effector paths   | ``endeffectors`` (all terminal links)    |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``IKGoal``                  |                                          | ``length`` (0.1), ``width`` (0.01)       |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``coordinates.Point``       |                                          | ``size`` (5)                             |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``coordinates.Direction``   |                                          | ``length`` (0.15)                        |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``coordinates.Frame``       |                                          | ``length`` (0.1), ``width`` (0.01)       |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``coordinates.Transform``   | Draws a curve between frames             |                                          |
+-----------------------------+------------------------------------------+------------------------------------------+
| ``ContactPoint``            |                                          | ``size`` (5), ``length`` (0.1)           |
+-----------------------------+------------------------------------------+------------------------------------------+

\* denotes a mandatory attribute.  Values in parentheses are defaults.

Note: ``color`` is always an accepted attribute.

Item path conventions and references
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  The world, if one exists, should be given the name ``'world'``.
-  Configurations and paths are drawn with reference to the first robot
   in the world.
-  The Simulator, if one exists, should be given the name ``'sim'``.
   Then, the vis module will save movies along simulation time instead
   of real (wall-clock) time.

For composite items like WorldModels, sub-items can be referred to by
passing a tuple or list of strings as the ``name`` argument to any of
these functions. For example, ``("world",robotname,linkname)`` refers
to a given link of a given robot inside the "world" item.

For example, if you've added a RobotWorld under the name ``'world'`` containing a
robot called ``'myRobot'``, then ``setColor(('world','myRobot'),0,1,0)`` will
turn the robot green. If ``'link5'`` is the robot's 5th link, then
``setColor(('world','myRobot','link5'),0,0,1)`` will turn the 5th
link blue.

Customizing item appearance
~~~~~~~~~~~~~~~~~~~~~~~~~~~

TODO: describe these functions

Animations
~~~~~~~~~~

The scene manager accepts animations for certain types of items.
Animations are currently supported for points, so3 elements, se3
elements, rigid objects, and robots.

-  ``vis.animate(name,animation,speed=1.0,endBehavior='loop')``: Sends
   an animation to the
   object. May be a Trajectory or a list of configurations.

   -  ``speed``: a modulator on the animation speed. If the animation is
      a list of
      milestones, it is by default run at 1 milestone per second.
   -  ``endBehavior``: either 'loop' (animation repeats forever) or
      'halt' (plays once).

-  ``vis.pauseAnimation(paused=True)``: Turns on/off animation globally.
-  ``vis.stepAnimation(amount)``: Moves forward the animation time by
   the given amount, in seconds.
-  \`vis.animationTime(newtime=None): Gets/sets the current animation
   time

   -  If newtime == None (default), this gets the animation time.
   -  If newtime != None, this sets a new animation time.



Making your own plugins
-----------------------

The :class:`~klampt.vis.glinterface.GLPluginInterface` class allows plugins functions to draw,
process mouse and keyboard input, etc. Users are also welcome to use
Klamp't object OpenGL calls in their own frameworks. For more
information, see the :class:`~klampt.vis.glinterface.GLPluginInterface` documentation 
and the simple example file
``Klampt-examples/Python/demos/gl_vis.py``.

For each GUI event (display, mousefunc, etc), the event cascades through
the plugin stack until one plugin's handler catches it by returning
True. Note: when implementing a plugin, you should not call any handler
functions yourself. Instead, the GUI will call these in response to OS
events. As a result, ``GLPluginInterface`` handlers are run inside the
visualization thread, and will not need to call the ``vis.lock()`` and
``vis.unlock()`` functions to modify Klamp't objects.

**Handlers:**

-  ``plugin.initialize()``: called once when OpenGL has been initialized
-  ``plugin.displayfunc()``: called each refresh cycle. No OpenGL calls
   have been set up here.
-  ``plugin.display()``: called each refresh cycle, with the background
   cleared and the current 3D perspective camera viewport set.
-  ``plugin.display_screen()``: called each refresh cycle, with the
   OpenGL viewport aligned to the window in orthographic projection.
   Used to draw text.
-  ``plugin.reshapefunc(w,h)``: called when the user or OS resizes the
   window.
-  ``plugin.keyboardfunc(c,x,y)``: called when the user types character
   c with the mouse at (x,y).
-  ``plugin.keyboardupfunc(c,x,y)``: called when the keyboard character
   c is released with the mouse at (x,y).
-  ``plugin.mousefunc(button,state,x,y)``: called when the mouse is
   clicked or released, with a given button, state, and position (x,y)
-  ``plugin.motionfunc(x,y,dx,dy)``: called when the mouse is moved to
   (x,y) with delta (dx,dy) from its previous position.
-  ``plugin.idle()``: called when the GUI is not working.
-  \`plugin.eventfunc(type,args=""): Generic hook for other events,
   e.g., button presses, from the GUI.
-  ``plugin.closefunc()``: called before the viewport is closed.

**Configuration functions** (these may be called during plugin setup,
especially during initialize())

-  ``plugin.add_action(callback,short_name,key,description=None)``:
   Defines a new generic GUI action. The action will be available in a
   menu in Qt or as keyboard commands in GLUT.
-  ``plugin.reshape(w,h)``: Asks to resize the viewport.
-  ``plugin.idlesleep(seconds)``: Asks to sleep the idle function.
   Usually called in idle to approximate a fixed-time loop."""
-  ``plugin.modifiers()``: Retrieves a list of currently pressed
   keyboard modifiers, i.e., combinations of 'ctrl', 'shift', 'alt'.
-  ``plugin.refresh()``: Asks the GUI for a redraw. If you are animating
   something, this must be called in ``idle()``.
-  ``plugin.draw_text(point,text,size=12,color=None)``: Draws text of
   the given size and color at the point (x,y) or (x,y,z).
-  ``plugin.click_ray(x,y)``: Returns the world-space ray
   (source,direction) associated with the camera click at x,y.
-  ``plugin.viewport()``: Retrieves the Viewport instance associated
   with the window.

Drawing your own world
~~~~~~~~~~~~~~~~~~~~~~~

You can completely override the standard vis scene manager using your own plugin,
but you will be responsible for all UI and OpenGL drawing functions.
Klampt provides convenience plugin base classes that show worlds and simulations in
the `klampt.vis.glrobotprogram <klampt.vis.glrobotprogram.html>`__ module. 

The following code shows how to subclass the :class:`~klampt.vis.glrobotprogram.GLWorldPlugin`
class, as well as the *mousefunc* and *motionfunc* callbacks to capture mouse clicks.

.. code:: python

    import klampt
    from klampt import vis
    from klampt.vis.glrobotprogram import GLWorldPlugin

    class MyPlugin(GLWorldPlugin):
      def __init__(self,world):
        GLWorldPlugin.__init__(self,world)

      def mousefunc(self,button,state,x,y):
        #Put your mouse handler here
        #the current example prints out the list of objects clicked whenever
        #you right click
        print "mouse",button,state,x,y
        if button==2:
          if state==0:
            print [o.getName() for o in self.click_world(x,y)]
            return
        GLWorldPlugin.mousefunc(self,button,state,x,y)

      def motionfunc(self,x,y,dx,dy):
        return GLWorldPlugin.motionfunc(self,x,y,dx,dy)
    
    world = klampt.WorldModel()
    if not world.readFile("Klampt-examples/data/athlete_plane.xml"):
      raise RuntimeError("Couldn't load world")
    vis.run(MyPlugin(world))


