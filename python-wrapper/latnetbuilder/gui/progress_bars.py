import ipywidgets as widgets

from .common import style_default, BaseGUIElement

def progress_bars():
    progress_bar_nets = widgets.FloatProgress(value=0., max=1., step=0.01, 
        bar_style='info', description='Number of nets explored:', 
        layout=widgets.Layout(display='none'), style=style_default)
    progress_bar_dim = widgets.FloatProgress(value=0., max=1., step=0.01, 
        bar_style='info', description='Number of dim explored:', 
        layout=widgets.Layout(display='none'), style=style_default)
    return BaseGUIElement(progress_bar_dim=progress_bar_dim,
                          progress_bar_nets=progress_bar_nets)
