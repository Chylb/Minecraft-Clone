#pragma once

#include <vector>
#include <map>

#include "UnbakedQuad.h"
#include "BakedModel.h"

class UnbakedModel {
public:
	UnbakedModel() = default;
	UnbakedModel(glm::vec3 from, glm::vec3 to, std::initializer_list<UnbakedQuad> quads);

	void AddQuad(UnbakedQuad quad);
	UnbakedModel& SetTexture(std::string oldTex, std::string newTex);

	UnbakedModel Translate(glm::vec3 offset);
	UnbakedModel Scale(glm::vec3 scale);
	UnbakedModel RotateX(float ang, glm::vec3 origin = { 8,8,8 });
	UnbakedModel RotateY(float ang, glm::vec3 origin = { 8,8,8 });
	UnbakedModel RotateZ(float ang, glm::vec3 origin = { 8,8,8 });

	BakedModel Bake();

private:
	std::vector<UnbakedQuad> m_quads;

	friend std::ostream& operator<<(std::ostream& os, const UnbakedModel& model);
};
