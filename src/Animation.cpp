#include "Animation.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include "Utility.h"

Animation::Animation()
{
	m_Camera = new FlyCamera();
}

bool Animation::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	Gizmos::create();

	glEnable(GL_DEPTH_TEST);

	m_BackgroundColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	
	m_FbxFile = new FBXFile();
	m_FbxFile->load("./models/characters/Marksman/Marksman.fbx");
	m_FbxFile->initialiseOpenGLTextures();

	generateGLMeshes(m_FbxFile);

	loadShaders("./shaders/skinnedVertex.glsl", "./shaders/skinnedFragment.glsl", &m_ProgramID);

	m_GizmoSize = 20;

	return true;
}

bool Animation::update()
{
	if (Application::update() == false)
	{
		return false;
	}



	float dt = glfwGetTime();
	m_Timer += dt;
	glfwSetTime(0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();
	vec4 m_White = vec4(1);
	vec4 m_Black = vec4(0, 0, 0, 1);
	Gizmos::addTransform(mat4(1));


	for (int i = 0; i <= m_GizmoSize; i++)
	{
		Gizmos::addLine(vec3(-m_GizmoSize / 2 + i, 0, -m_GizmoSize / 2), vec3(-m_GizmoSize / 2 + i, 0, m_GizmoSize / 2), i == m_GizmoSize / 2 ? m_White : m_Black);
		Gizmos::addLine(vec3(-m_GizmoSize / 2, 0, -m_GizmoSize / 2 + i), vec3(m_GizmoSize / 2, 0, -m_GizmoSize / 2 + i), i == m_GizmoSize / 2 ? m_White : m_Black);
	}

	FBXSkeleton* skele = m_FbxFile->getSkeletonByIndex(0);
	FBXAnimation* anim = m_FbxFile->getAnimationByIndex(0);

	for (unsigned int i = 0; i < skele->m_boneCount; i++)
	{
		skele->m_nodes[i]->updateGlobalTransform();
		mat4 nodeGlobal = skele->m_nodes[i]->m_globalTransform;
		vec3 nodePos = nodeGlobal[3].xyz;
		Gizmos::addAABBFilled(nodePos, vec3(0.5f), vec4(1, 0, 0, 1), &nodeGlobal);
		if (skele->m_nodes[i]->m_parent != nullptr)
		{
			vec3 parentpos = skele->m_nodes[i]->m_parent->m_globalTransform[3].xyz;
			Gizmos::addLine(nodePos, parentpos, vec4(0, 1, 0, 1));
		}
	}
	
	evaluateSkeleton(anim, skele, m_Timer);

	updateBones(skele);
	m_Camera->update(dt);

	return true;
}

void Animation::draw()
{
	glClearColor(m_BackgroundColor.x, m_BackgroundColor.y, m_BackgroundColor.z, m_BackgroundColor.w);
	glUseProgram(m_ProgramID);
	int projViewHandle = glGetUniformLocation(m_ProgramID, "projectionView");
	if (projViewHandle >= 0)
	{
		glUniformMatrix4fv(projViewHandle, 1, false, (float*)&m_Camera->getProjectionView());
	}

	int diffuseUniform = glGetUniformLocation(m_ProgramID, "diffuse");
	glUniform1i(diffuseUniform, 0);

	FBXSkeleton* skele = m_FbxFile->getSkeletonByIndex(0);
	int bonesUniform = glGetUniformLocation(m_ProgramID, "bones");
	glUniformMatrix4fv(bonesUniform, skele->m_boneCount, GL_FALSE, (float*)skele->m_bones);


	Gizmos::draw(m_Camera->getProjectionView());	

	for (unsigned int i = 0; i < m_Meshes.size(); i++)
	{
		FBXMeshNode* currMesh = m_FbxFile->getMeshByIndex(i);
		mat4 worldTransform = currMesh->m_globalTransform;
		FBXMaterial* meshMaterial = currMesh->m_material;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, meshMaterial->textures[FBXMaterial::DiffuseTexture]->handle);
		
		int worldUniform = glGetUniformLocation(m_ProgramID, "world");
		glUniformMatrix4fv(worldUniform, 1, GL_FALSE, (float*)&worldTransform);
		
		glBindVertexArray(m_Meshes[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_Meshes[i].m_IndexCount, GL_UNSIGNED_INT, 0);
		

	}

	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void Animation::shutdown()
{
	Gizmos::destroy();

	m_FbxFile->unload();

	Application::shutdown();
}

void Animation::generateGLMeshes(FBXFile* a_fbx)
{
	unsigned int meshCount = a_fbx->getMeshCount();
	m_Meshes.resize(meshCount);

	for (unsigned int i = 0; i < meshCount; i++)
	{
		FBXMeshNode* currMesh = a_fbx->getMeshByIndex(i);

		m_Meshes[i].m_IndexCount = currMesh->m_indices.size();
		glGenBuffers(1, &m_Meshes[i].m_VBO);
		glGenBuffers(1, &m_Meshes[i].m_IBO);
		glGenVertexArrays(1, &m_Meshes[i].m_VAO);

		glBindVertexArray(m_Meshes[i].m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_Meshes[i].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex)* currMesh->m_vertices.size(), currMesh->m_vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Meshes[i].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * currMesh->m_indices.size(), currMesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //texCoord
		glEnableVertexAttribArray(2); //bone indices
		glEnableVertexAttribArray(3); //bone weights


		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	}
}

void Animation::evaluateSkeleton(FBXAnimation* a_anim, FBXSkeleton* a_skeleton, float a_timer)
{
	float fps = 24.0f;
	int current = (int)(a_timer * fps);
	
	//loop through all the tracks
	for (unsigned int i = 0; i < a_anim->m_trackCount; i++)
	{	
		//wrap back to the start of the track if we've gone too far
		int trackFrameCount = a_anim->m_tracks[i].m_keyframeCount;
		int trackFrame = current % trackFrameCount;

		//find what keyframes are currently effecting bones
		FBXKeyFrame currFrame = a_anim->m_tracks[i].m_keyframes[trackFrame];
		FBXKeyFrame nextFrame = a_anim->m_tracks[i].m_keyframes[(trackFrame + 1) % trackFrameCount];
			
		float frameFlip = a_timer - (current / fps);
		float t = frameFlip * fps;
		
		//interpolate between those keyframes to generate the matrix for current pose
		vec3 newPos = glm::mix(currFrame.m_translation, nextFrame.m_translation, t);
		vec3 newScale = glm::mix(currFrame.m_scale, nextFrame.m_scale, t);
		glm::quat newRot = glm::slerp(currFrame.m_rotation, nextFrame.m_rotation, t);
		
		mat4 local = glm::translate(newPos) * glm::toMat4(newRot) * glm::scale(newScale);
		
		// get the right bone for given track
		int boneIndex = a_anim->m_tracks[i].m_boneIndex;

		//set the FBXnode's local transforms to match
		if (boneIndex < a_skeleton->m_boneCount)
		{
			a_skeleton->m_nodes[boneIndex]->m_localTransform = local;
		}
			

	}


}

void Animation::updateBones(FBXSkeleton* a_skeleton)
{
	// loop through the nodes in the skeleton

	for (unsigned int i = 0; i < a_skeleton->m_boneCount; i++)
	{
		//generate their global transforms
		int parentIndex = a_skeleton->m_parentIndex[i];

		if (parentIndex == -1)
		{
			a_skeleton->m_bones[i] = a_skeleton->m_nodes[i]->m_localTransform;
		}
		else
		{
			a_skeleton->m_bones[i] = a_skeleton->m_bones[parentIndex] * a_skeleton->m_nodes[i]->m_localTransform;
		}

	}
	for (unsigned int i = 0; i < a_skeleton->m_boneCount; i++)
	{
		//multiply the global transforms by the inverse bind pose
		a_skeleton->m_bones[i] = a_skeleton->m_bones[i] * a_skeleton->m_bindPoses[i];
	}

}