#include "logic.h"

void logic::initialize(graphics& graphics)
{
	m_field.initialize(&graphics);
}

void logic::update()
{
	m_field.update();
}

void logic::finalize()
{

}