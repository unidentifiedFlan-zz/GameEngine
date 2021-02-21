#include "TestScene.h"
#include "OBJLoader.h"
#include "Plane.h"
#include "Cube.h"
#include "Liquid.h"


std::shared_ptr<Scene> TestScene::generate(std::vector<Texture> &textures)
{
	std::shared_ptr<Camera> camera( new Camera(glm::vec3(0.0f, 0.0f, 3.0f) ) );
	std::shared_ptr<Scene> scene( new Scene() );
	scene->setCamera(camera);
	scene->addLights(createLights(camera));
	scene->addObjects(createObjects(textures));
//	TextureLoader texLoader;
//	OBJLoader loader(texLoader);
//	std::shared_ptr<RenderObject> bag = loader.load("Backpack/backpack.obj");
//	bag->transform.translate( glm::vec3(0.0f, 1.0f, 0.0f) );
//	scene->addObject( bag );

	return scene;
}

std::vector<glm::vec3> pointLightPositions = {
//	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(0.0f,  0.0f, -4.0f),
//	glm::vec3(-4.0f, 0.0f, -4.0f)
};

std::vector<std::shared_ptr<Light>> TestScene::createLights(const std::shared_ptr<Camera>& camera)
{
	std::vector<std::shared_ptr<Light>> lights;

	for (int i = 0; i < pointLightPositions.size(); ++i)
	{
		std::shared_ptr<Mesh> cube( new Cube() );
		std::shared_ptr<PointLight> pLight( new PointLight() );
		pLight->setMesh( cube );
		pLight->transform.translate(pointLightPositions[i]);
		pLight->transform.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
		pLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
		pLight->diffuse = glm::vec3(0.3f, 0.0f, 0.3f);
		pLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
		pLight->constant = 1.0f;
		pLight->linear = 0.09f;
		pLight->quadratic = 0.032f;

		lights.push_back(pLight);
	}
	
	std::shared_ptr<DirectionalLight> dLight( new DirectionalLight() );
	dLight->direction = glm::normalize(glm::vec3(-1.0f, -0.1f, 0.0f));
	dLight->ambient = glm::vec3(0.5f, 0.5f, 0.5f);
	dLight->diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	dLight->specular = glm::vec3(0.1f, 0.1f, 0.1f);
	dLight->transform.translate(glm::vec3(12.0f, 1.0f, 0.0f));

	lights.push_back(dLight);

	return lights;
}

std::vector<std::shared_ptr<RenderObject>> TestScene::createObjects(std::vector<Texture> &textures)
{
	std::vector<std::shared_ptr<RenderObject>> objects;

	TextureLoader texLoader;

	OBJLoader loader(texLoader);
	std::shared_ptr<RenderObject> crate = loader.load("Cube/cube.obj");
    objects.push_back(crate);

	std::shared_ptr<RenderObject> room( new RenderObject() );
	std::shared_ptr<RenderObject> floor( new RenderObject() );
	floor->name = "floor";
	std::shared_ptr<Mesh> plane( new Plane() );
	std::dynamic_pointer_cast<Plane>( plane )->setTextureScale(2.0f, 2.0f);
	floor->setMesh(plane);
	floor->transform.translate(glm::vec3(0.0f, -0.5f, 0.0f));
	floor->transform.setScale(glm::vec3(10.0f, 10.0f, 10.0f));
    floor->transform.rotate(glm::vec3(1.0f, 0.0f, 0.0f), -glm::pi<float>()/2);
	Texture planeTex = texLoader.loadMaterialTexture("Textures/wall.jpg", Texture::Type::DIFFUSE);
	floor->material.diffuseMap = planeTex;
	room->addChild(floor);

	std::shared_ptr<Mesh> wall1Mesh( new Plane() );
	std::dynamic_pointer_cast<Plane>( wall1Mesh )->setTextureScale(2.0f, 1.0f);
	std::shared_ptr<RenderObject> wall1( new RenderObject() );
	wall1->name = "wall1";
	wall1->setMesh(wall1Mesh);
	wall1->transform.translate(glm::vec3(0.0f, 0.75f, -5.0f));
	wall1->transform.setScale(glm::vec3(10.0f, 2.5f, 1.0f));
	Texture wall1Diff = texLoader.loadMaterialTexture("Textures/brickwall.jpg", Texture::Type::DIFFUSE);
	Texture wall1Norm = texLoader.loadMaterialTexture("Textures/brickwall_normal.jpg", Texture::Type::NORMAL);
	wall1->material.diffuseMap = wall1Diff;
	wall1->material.normalMap = wall1Norm;
	room->addChild(wall1);

	std::shared_ptr<Mesh> wall2Mesh( new Plane() );
	std::dynamic_pointer_cast<Plane>( wall2Mesh )->setTextureScale(2.0f, 1.0f);
	std::shared_ptr<RenderObject> wall2( new RenderObject() );
	wall2->name = "wall2";
	wall2->setMesh(wall2Mesh);
	wall2->transform.translate(glm::vec3(-5.0f, 0.75f, 0.0f));
	wall2->transform.setScale(glm::vec3(10.0f, 2.5f, 1.0f));
	wall2->transform.rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::pi<float>()/2);
	Texture wall2Diff = texLoader.loadMaterialTexture("Textures/Concrete/Concrete003_1K_Color.png", Texture::Type::DIFFUSE);
	wall2->material.diffuseMap = wall2Diff;
	Texture wall2Norm = texLoader.loadMaterialTexture("Textures/Concrete/Concrete003_1K_Normal.png", Texture::Type::NORMAL);
	wall2->material.normalMap = wall2Norm;
	room->addChild(wall2);

	std::shared_ptr<Mesh> wall3Mesh( new Plane() );
	std::dynamic_pointer_cast<Plane>( wall3Mesh )->setTextureScale(2.0f, 1.0f);
	std::shared_ptr<RenderObject> wall3( new RenderObject() );
	wall3->name = "wall3";
	wall3->setMesh(wall3Mesh);
	wall3->transform.translate(glm::vec3(0.0f, 0.75f, 5.0f));
	wall3->transform.setScale(glm::vec3(10.0f, 2.5f, 1.0f));
	wall3->transform.rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::pi<float>());
	wall3->material.diffuseMap = wall2Diff;
	wall3->material.normalMap = wall2Norm;
	room->addChild(wall3);

	std::shared_ptr<Mesh> ceilingMesh( new Plane() );
	std::dynamic_pointer_cast<Plane>( ceilingMesh )->setTextureScale(2.0f, 2.0f);
	std::shared_ptr<RenderObject> ceiling( new RenderObject() );
	ceiling->name = "ceiling";
	ceiling->setMesh(ceilingMesh);
	ceiling->transform.translate(glm::vec3(0.0f, 2.0f, 0.0f));
	ceiling->transform.setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	ceiling->transform.rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::pi<float>()/2);
	Texture ceilingDiff = texLoader.loadMaterialTexture("Textures/WoodBoards/Planks012_1K_Color.png", Texture::Type::DIFFUSE);
	ceiling->material.diffuseMap = ceilingDiff;
	Texture ceilingNorm = texLoader.loadMaterialTexture("Textures/WoodBoards/Planks012_1K_Normal.png", Texture::Type::NORMAL);
	ceiling->material.normalMap = ceilingNorm;
	room->addChild(ceiling);

	objects.push_back(room);

	std::shared_ptr<Mesh> waterPlane( new Plane(100) );
	std::shared_ptr<Liquid> water( new Liquid() );
	water->name = "water";
	water->setMesh(waterPlane);
	water->transform.translate(glm::vec3(20.0f, -5.0f, 0.0f));
	water->transform.rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::pi<float>()/2);
	water->transform.setScale(glm::vec3(200.0f, 200.0f, 100.0f));
	water->material.diffuse = glm::vec3(0.1f, 0.4f, 0.9f);
	water->material.ambient = glm::vec3(0.1f, 0.4f, 0.9f);
	water->material.specular = glm::vec3(0.1f, 0.4f, 0.5f);
	water->material.diffuseMap = wall2Diff;
	water->material.shader = Shader("shader.vs", "waterShader.fs");

	DirectionalWave wave1(/*wavelength*/0.4f, /*speed*/0.0025f, /*amplitude*/0.02f, /*steepness*/0.01f, /*direction*/glm::vec3(1.0f, 0.3f, 0.0f));
	DirectionalWave wave2(/*wavelength*/0.005f, /*speed*/0.005f, /*amplitude*/0.001f, /*steepness*/0.001f, /*direction*/glm::vec3(1.0f, -0.1f, 0.0f));
	DirectionalWave wave3(/*wavelength*/0.01f, /*speed*/0.0035f, /*amplitude*/0.008f, /*steepness*/0.001f, /*direction*/glm::vec3(1.0f, -1.0f, 0.0f));
	DirectionalWave wave4(/*wavelength*/0.002f, /*speed*/0.001f, /*amplitude*/0.001f, /*steepness*/0.01f, /*direction*/glm::vec3(0.7f, 0.4f, 0.0f));
	DirectionalWave wave5(/*wavelength*/0.0025f, /*speed*/0.001f, /*amplitude*/0.001f, /*steepness*/0.01f, /*direction*/glm::vec3(1.0f, 0.0f, 0.0f));

	DirectionalWave wave6(/*wavelength*/0.0001f, /*speed*/0.00001f, /*amplitude*/0.00001f, /*steepness*/0.1f, /*direction*/glm::vec3(1.0f, 0.0f, 0.0f));
	DirectionalWave wave7(/*wavelength*/0.0002f, /*speed*/0.00001f, /*amplitude*/0.00001f, /*steepness*/0.1f, /*direction*/glm::vec3(1.0f, -0.03f, 0.0f));
	DirectionalWave wave8(/*wavelength*/0.00028f, /*speed*/0.00005f, /*amplitude*/0.00005f, /*steepness*/0.1f, /*direction*/glm::vec3(1.0f, 0.4f, 0.0f));
	DirectionalWave wave9(/*wavelength*/0.00015f, /*speed*/0.00003f, /*amplitude*/0.000023f, /*steepness*/0.1f, /*direction*/glm::vec3(1.0f, 0.1f, 0.0f));
	DirectionalWave wave10(/*wavelength*/0.00008f, /*speed*/0.0000085f, /*amplitude*/0.00003f, /*steepness*/0.1f, /*direction*/glm::vec3(1.0f, -0.3f, 0.0f));
	DirectionalWave wave11(/*wavelength*/0.00092f, /*speed*/0.000014f, /*amplitude*/0.000009f, /*steepness*/0.1f, /*direction*/glm::vec3(1.0f, -0.5f, 0.0f));
	DirectionalWave wave12(/*wavelength*/0.00077f, /*speed*/0.000006f, /*amplitude*/0.000015f, /*steepness*/0.1f, /*direction*/glm::vec3(1.0f, 0.2f, 0.0f));


	water->addWave(wave1);
	water->addWave(wave2);
	water->addWave(wave3);
	water->addWave(wave4);
	water->addWave(wave5);
	water->addWave(wave6);
	water->addWave(wave7);
	water->addWave(wave8);
	water->addWave(wave9);
	water->addWave(wave10);
	water->addWave(wave11);
	water->addWave(wave12);


	objects.push_back(water);

	textures = texLoader.loaded_textures;

	return objects;
}