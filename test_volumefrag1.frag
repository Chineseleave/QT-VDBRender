
#version 450 core
layout(location = 0) out vec4 vFragColor;	//fragment shader output

in vec3 mBBoxmin;
in vec3 mBBoxmax;

uniform vec3 camPos;
in vec3 vPosition;
uniform sampler3D volume;
const int stepMaxNum = 128;

float remap(float x, float low1, float high1, float low2, float high2){
    return low2 + (x - low1) * (high2 - low2) / (high1 - low1);
}

vec2 rayBoxDst(vec3 BoundsMin,vec3 BoundsMax , vec3 rayOrigin ,vec3 rayDir)
{
    vec3 t0 = (BoundsMin - rayOrigin) / rayDir;
    vec3 t1 = (BoundsMax - rayOrigin) / rayDir;

    vec3 tmin = min(t0,t1);
    vec3 tmax = max(t0,t1);

    float dstA = max(max(tmin.x,tmin.y),tmin.z);//判别Boundbox那个轴最大
    float dstB = min(min(tmax.x,tmax.y),tmax.z);

    float dstToBox = max(0,dstA);
    float dstInsideBox = max(0,dstB - dstToBox);

    return vec2(dstToBox,dstInsideBox);
}


vec4 rayMarching(vec3 BoundsMin,vec3 BoundsMax,vec3 rayOrigin,vec3 rayDir,sampler3D volumeTex)
{
    vec2 hitInfo = rayBoxDst(BoundsMin,BoundsMax , rayOrigin ,rayDir);

    float stepSize = hitInfo.y / stepMaxNum;

    vec3 samplePoint = rayOrigin + rayDir * (hitInfo.x);



    float transmittance = 1;//透光率

    for(int step = 0; step <stepMaxNum;step++)
    {
        float den = texture(volumeTex,
                            vec3(
                                remap(samplePoint.x, BoundsMin.x, BoundsMax.x, 0, 1),
                                remap(samplePoint.y, BoundsMin.y, BoundsMax.y, 0, 1),
                                remap(samplePoint.z, BoundsMin.z, BoundsMax.z, 0, 1)
                                )
                            ).r * 1.0 * stepSize;

        if(den >= 0.0) {
            transmittance *= exp(-den*1.2);
        }

        /*
        else{
            transmittance *= exp(-1.2);
        }*/

        samplePoint += rayDir * stepSize;

    }

    return vec4(transmittance,transmittance,transmittance,1-transmittance);
}

void main()
{
    vec3 rayDir = normalize(vPosition-camPos);
    vec4 cd = rayMarching(mBBoxmin,mBBoxmax,camPos,rayDir,volume);
    vFragColor = cd;
}

