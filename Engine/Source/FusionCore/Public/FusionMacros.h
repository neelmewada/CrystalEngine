#pragma once

#define FNew(WidgetClass)\
	(*CreateObject<WidgetClass>(nullptr, #WidgetClass))
	
