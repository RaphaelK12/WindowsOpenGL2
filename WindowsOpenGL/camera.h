#pragma once
class camera :
	public objetob
{
public:
	camera(void);
	camera(objetob* parent, vec3 localPos, vec3 lookAt, vec3 vectorUp,
		   float Aspect = 1.0f, float NearClip = 0.1f, float FarClip = 100.f, float Left = -1.f, float Right = 1.f,
		   float Botton = -1.f, float Top = 1.f, float Fov = 1.5f
		   );

	~camera(void);
	//objetob* parent;
	float nearClip;
	float farClip;
	float left;
	float right;
	float botton;
	float top;
	float fov;
	float aspect;
	vec3 dir;
	vec3 up;
	//vec3 rot;

	void calcMatrix();
	void moveF(float delta);
	void moveB(float delta);
	void moveL(float delta);
	void moveR(float delta);

	void rotate(vec3 v);

};

extern camera* activecamera;


