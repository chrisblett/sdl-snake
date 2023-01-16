#include "WorldUtil.h"

#include "../Engine/Math/Math.h"
#include "../Engine/Math/Vector2.h"
#include "../Engine/SDLAppRenderer.h"

void WorldUtil::DrawRectAtCell(const SDLAppRenderer& renderer, const Vector2& cellPos, float rectScale)
{
	// Calculate rect top-left pos relative to cell top-left
	Vector2 centeredPos = Math::GetCenteredPosition(cellPos, rectScale, rectScale);

	// Want to draw around the center of the cell so add half a unit
	// in each axis so we are centered around its midpoint
	renderer.FillRect(renderer.WorldToScreen(
		centeredPos.x + .5f,
		centeredPos.y + .5f,
		rectScale,
		rectScale)
	);
}

float WorldUtil::WorldVecToAngle(const Vector2& v)
{
	// Flip the y-coord to work with trig coord system
	return Math::ToDegrees(atan2f(-v.y, v.x));
}