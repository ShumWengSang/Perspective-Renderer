
in VS_OUT
{
    vec3 fragDiffuse;
    vec3 fragNormal;

} fs_in;


out vec3 fragColor;

void main()
{
    vec3 lightVector = vec3( 0.0, 0.0, 1.0 );
    float N_dot_L = max( dot( normalize(fs_in.fragNormal), normalize(lightVector)), 0.0f );
	fragColor = fs_in.fragDiffuse * N_dot_L;
}
