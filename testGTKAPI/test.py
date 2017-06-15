#!/usr/bin/python
from gi.repository import Gtk

class MyWindow(Gtk.Window):

    def __init__(self):
        Gtk.Window.__init__(self, title="Hello World Printing")
        self.button = Gtk.Button(label="Print A Rectangle")
        self.button.connect("clicked", self.on_button_clicked)
        self.add(self.button)

    def on_button_clicked(self, widget):
        pd = Gtk.PrintOperation()
        pd.set_n_pages(1)
        pd.connect("draw_page", self.draw_page)
        result = pd.run(
            Gtk.PrintOperationAction.PRINT_DIALOG, None)
        print result  # handle errors etc.

    def draw_page(self, operation=None, context=None, page_nr=None):
        ctx = context.get_cairo_context()
        w = context.get_width()
        h = context.get_height()
        ctx.set_source_rgb(0.5, 0.5, 1)
        ctx.rectangle(w*0.1, h*0.1, w*0.8, h*0.8)
        ctx.stroke()
        return


win = MyWindow()
win.connect("delete-event", Gtk.main_quit)
win.show_all()
Gtk.main()
