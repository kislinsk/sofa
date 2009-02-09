//varying vec3 normalVec;
//varying vec3 viewVec;
varying vec4 vPositionW;
varying vec3 vNormalW;

varying vec4 diffuse, ambient, ambientGlobal, specular;
varying vec3 lightDir, normalView, halfVector;
varying float dist;
varying vec3 spotDir;
varying vec3 restPositionW;

attribute vec3 restPosition; 

void main()
{	
	gl_Position = ftransform();
	gl_FrontColor = gl_FrontMaterial.diffuse;
	gl_BackColor = gl_BackMaterial.diffuse;

	//gl_TexCoord[0] = gl_MultiTexCoord0;
	
	// Compute position and normal in world space
	vec3 positionW = gl_Vertex.xyz;
	vec3 normalW = normalize(gl_NormalMatrix * gl_Normal);
	restPositionW = restPosition;
	
	vec3 eyePositionW = gl_ModelViewMatrixInverse[3].xyz;
	
	// Compute the incident and reflected vectors
	//vec3 I = positionW - eyePositionW;
	//reflectVec = reflect(I, normalW);
	//normalVec = normalW;
	//viewVec = I;
	//vPosition = gl_ModelViewMatrix * gl_Vertex;
	vPositionW = gl_Vertex;
	vNormalW = normalize(gl_Normal);
	
	vPositionW.x = 0.5 * (1.0 + vPositionW.x);
	vPositionW.y = 0.5 * (1.0 + vPositionW.y);
	vPositionW.z = 0.5 * (1.0 + vPositionW.z);
	vPositionW.w = 1.0;
	
	normalView = gl_NormalMatrix * gl_Normal;
	vec4 ecPos = gl_ModelViewMatrix * gl_Vertex;
	vec3 aux = vec3(gl_LightSource[0].position-ecPos);
	dist = length(aux);
	lightDir = normalize(aux);

	
	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;
	specular = gl_FrontMaterial.specular * gl_LightSource[0].specular;
	
	//eyeVec = -vec3(ecPos);
	spotDir = gl_LightSource[0].spotDirection;
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);
}
