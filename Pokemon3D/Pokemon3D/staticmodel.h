#pragma once
#include "pokedef.h"
#include "body.h"

class StaticModel
{
public:
	StaticModel(){}
	StaticModel(const Body& body, const float3& pos, const float3& rot):
	m_body(body),
	m_pos(pos),
	m_rot(rot){};
	~StaticModel(){}

	void setBody(const Body& body) { m_body = body; }

	void changeTexCoords(const float2 coords[6])
	{
		for (size_t i = 0; i < 6; ++i)
		{
			Body::vertex_t v = m_body.getVertex(i);
			v.u = coords[i].x;
			v.v = coords[i].y;
			m_body.setVertex(i, v);
		}
	}

	void setPos(const float3& pos) { m_pos = pos; }
	void setX(const float x) { m_pos.x = x; }
	void setY(const float y) { m_pos.y = y; }
	void setZ(const float z) { m_pos.z = z; }

	void setRot(const float3& rot) { m_rot = rot; }
	void setRotX(const float rotX) { m_rot.x = rotX; }
	void setRoty(const float rotY) { m_rot.y = rotY; }
	void setRotZ(const float rotZ) { m_rot.z = rotZ; }
	
	Body* modBodyPointer() { return &m_body; }
	const Body& getBody() const { return m_body; }
	const float3& getPos() const { return m_pos; }
	const float3& getRot() const { return m_rot; }

private:
	Body m_body;
	float3 m_pos;
	float3 m_rot;
};