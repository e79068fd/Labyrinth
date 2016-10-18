#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform vec4 color;

varying vec3 normal;
varying vec3 pos;

void main()
{
    vec3 light_position = vec3(0.0, 0.0, 0.0);
    vec3 light_intensities = vec3(0.95, 0.95, 0.95);
    float light_attenuation = 0.6;
    float ambientCoefficient = 0.85;

    float materialShininess = 64.0;
    vec3 materialSpecularColor = vec3(0.9, 0.9, 0.9);

    vec3 surfaceToLight = normalize(light_position - pos);
    vec3 surfaceToCamera = normalize(-pos);

    //ambient
    vec3 ambient = ambientCoefficient * color.rgb * light_intensities;

    //diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * color.rgb * light_intensities;

    //specular
    float specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), materialShininess);
    vec3 specular = specularCoefficient * materialSpecularColor * light_intensities;

    //attenuation
    float distanceToLight = length(light_position - pos);
    float attenuation = 1.0 / (1.0 + light_attenuation * pow(distanceToLight, 2.0));

    //linear color (color before gamma correction)
    vec3 linearColor = ambient + attenuation*(diffuse + specular);

    //final color (after gamma correction)
    vec3 gamma = vec3(1.0/2.2);
    gl_FragColor = vec4(pow(linearColor, gamma), color.a);

    //gl_FragColor = color;
}
