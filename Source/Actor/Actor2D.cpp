#include "Actor2D.h"
#include "Game.h"

Actor2D::Actor2D(Game* game, const XMFLOAT2& pos, const XMFLOAT2& vel)
	:Actor(game)
	, m_pos(pos)
	, m_vel(vel)
{

}

Actor2D::~Actor2D()
{
}