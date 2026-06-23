#include "loading.h"

unsigned int shaderLoad(const char* vsPath, const char* fsPath) {
    FILE *vsFile;
    FILE *fsFile;
    long int vsLength;
    long int fsLength;
    char *vsContent;
    char *fsContent;
    unsigned int vs, fs, program;
    int compiled;
    
    vsFile = fopen(vsPath, "r");
    if (!vsFile) {
        printf("%s does not exist\n", vsPath);
        return 0;
    } 
    fseek(vsFile, 0, SEEK_END);
    vsLength = ftell(vsFile) + 4;
    fseek(vsFile, 0, SEEK_SET);
    vsContent = (char*)malloc((vsLength+1)*sizeof(char));
    char line[256];
    vsContent[0] = '\0';
    while (fgets(line, 256, vsFile)) {
        if (strstr(line, "#version") && isGLES) {
            strcat(vsContent, "#version 300 es\n");
        } else {
            strcat(vsContent, line);
        }
    }
    vsContent[vsLength] = '\0';
    fclose(vsFile);
    const char* vsSource = vsContent;

    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSource, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &compiled);
    if (compiled) printf("compiled %s\n", vsPath);
    else printf("failed compiling %s\n", vsPath);

    free(vsContent);


    fsFile = fopen(fsPath, "r");
    if (!fsFile) {
        printf("%s does not exist\n", fsPath);
        return 0;
    }
    fseek(fsFile, 0, SEEK_END);
    fsLength = ftell(fsFile) + 4;
    fseek(fsFile, 0, SEEK_SET);
    fsContent = (char*)malloc((fsLength+1)*sizeof(char));
    fsContent[0] = '\0';
    while (fgets(line, 256, fsFile)) {
        if (strstr(line, "#version") && isGLES) {
            strcat(fsContent, "#version 300 es\n");
        } else {
            strcat(fsContent, line);
        }
    }
    fsContent[fsLength] = '\0';
    fclose(fsFile);
    const char* fsSource = fsContent;

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSource, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &compiled);
    if (compiled) printf("compiled %s\n", fsPath);
    else {
        printf("failed compiling %s\n", fsPath);
        char buffer[256];
        int loglength;
        glGetShaderInfoLog(fs, 256, &loglength, buffer);
        printf("%s\n", buffer);
    }

    free(fsContent);

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


void textureLoad(textureinfo *texture, const char* texPath, unsigned int format, bool flipped) {
    unsigned int t;
    int width, height, channels;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(flipped);
    unsigned char *imageData = stbi_load(texPath, &width, &height, &channels, 0);
    if (!format) {
        format = channels == 3 ? GL_RGB : GL_RGBA;
    }
    if (imageData) {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
    } else {
        printf("failed to load texture\n");
    }
    stbi_image_free(imageData);
    texture->tex = t;
    texture->width = width;
    texture->height = height;
}

unsigned int modelLoadOBJ(modelinfo *dest, const char* modelPath) {
    unsigned int vcount, vtcount, vncount, fcount;
    vcount = vtcount = vncount = fcount = 0;
    size_t vsize, vtsize, vnsize, vertssize;
    vsize = vtsize = vnsize = vertssize = 1024;
    FILE *modelFile;
    
    modelFile = fopen(modelPath, "r");
    if (!modelFile) {
        printf("%s does not exist\n", modelPath);
        return 0;
    } 

    char line[256];
    char *token;
    float *v = malloc(vsize*sizeof(float));
    float *vt = malloc(vtsize*sizeof(float));
    float *vn = malloc(vnsize*sizeof(float));
    float *verts = malloc(vertssize*sizeof(float));

    while (fgets(line, sizeof(line), modelFile)) {
        if (strncmp(line, "v ", 2) == 0) {
            token = strtok(line, " ");
            token = strtok(NULL, " ");
            if (vsize < vcount+3) {
                vsize *= 2;
                float *tmp = realloc(v, vsize*sizeof(float));
                v = tmp;
            }
            while (token) {
                v[vcount] = atof(token);
                token = strtok(NULL, " ");
                vcount++;
            }
        } else if (strncmp(line, "vt", 2) == 0) {
            token = strtok(line, " ");
            token = strtok(NULL, " ");
            if (vtsize < vtcount+3) {
                vtsize *= 2;
                float *tmp = realloc(vt, vtsize*sizeof(float));
                vt = tmp;
            }
            while (token) {
                vt[vtcount] = atof(token);
                token = strtok(NULL, " ");
                vtcount++;
            }
        } else if (strncmp(line, "vn", 2) == 0) {
            token = strtok(line, " ");
            token = strtok(NULL, " ");
            if (vnsize < vncount+3) {
                vnsize *= 2;
                float *tmp = realloc(vn, vnsize*sizeof(float));
                vn = tmp;
            }
            while (token) {
                vn[vncount] = atof(token);
                token = strtok(NULL, " ");
                vncount++;
            }
        } else if (strncmp(line, "f ", 2) == 0) {
            char *saveOuter;
            token = strtok_r(line, " ", &saveOuter);
            token = strtok_r(NULL, " ", &saveOuter);
            while (token) {
                int vi, vti, vni;
                if (vertssize < fcount*8+8) {
                    vertssize *= 2;
                    float *tmp = realloc(verts, vertssize*sizeof(float));
                    verts = tmp;
                }
                sscanf(token, "%d/%d/%d", &vi, &vti, &vni);
                vi--;
                vti--;
                vni--;
                verts[8*fcount] = v[3*vi];
                verts[8*fcount+1] = v[3*vi+1];
                verts[8*fcount+2] = v[3*vi+2];
                verts[8*fcount+3] = vt[2*vti];
                verts[8*fcount+4] = vt[2*vti+1];
                verts[8*fcount+5] = vn[3*vni];
                verts[8*fcount+6] = vn[3*vni+1];
                verts[8*fcount+7] = vn[3*vni+2];

                fcount++;
                token = strtok_r(NULL, " ", &saveOuter);                
            }
        }
    }

    free(v);
    free(vt);
    free(vn);
    fclose(modelFile);
    dest->verts = verts;
    dest->size = fcount;
    dest->modelFormat = MODEL_OBJ;
    printf("loaded %s\n", modelPath);
    return 1;
}

void modelLoadGLTF(modelinfo *dest, const char *modelPath) {
    FILE *modelFile = fopen(modelPath, "r");
    fseek(modelFile, 0, SEEK_END);
    size_t modelFileLength = ftell(modelFile);
    fseek(modelFile, 0, SEEK_SET);
    char *modelFileData = malloc(modelFileLength + 1);
    fread(modelFileData, 1, modelFileLength, modelFile);

    int jsonPropertyCount = 0;
    for (int i = 0; i < modelFileLength; i++) {
        if (modelFileData[i] == ':' || modelFileData[i] == ',' || modelFileData[i] == '{') {
            jsonPropertyCount++;
        }
    }
    fclose(modelFile);
    modelFileData[modelFileLength] = '\0';

    json_t jsonMemory[jsonPropertyCount];
    json_t const* parent = json_create(modelFileData, jsonMemory, jsonPropertyCount);
    if (parent == NULL) {
        printf("loading %s failed\n", modelPath);
        return;
    }
    struct bufferView {
        int componentType;
        int count;
        int type;
        int buffer;
        int byteLength;
        int byteOffset;
    };
    int bufferViewAmount = 64;
    struct bufferView bufferViews[bufferViewAmount];

    int locPos, locNormal, locTexCoord, locIndices;
    for (json_t const* mesh = json_getChild(json_getProperty(parent, "meshes")); mesh; mesh = json_getSibling(mesh)) {
        json_t const* attributes = json_getProperty(json_getChild(json_getProperty(mesh, "primitives")), "attributes");
        locPos = json_getInteger(json_getProperty(attributes, "POSITION"));
        locNormal = json_getInteger(json_getProperty(attributes, "NORMAL"));
        locTexCoord = json_getInteger(json_getProperty(attributes, "TEXCOORD_0"));
        locIndices = json_getInteger(json_getProperty(json_getChild(json_getProperty(mesh, "primitives")), "indices"));
    }
    //printf("meshes\n");

    for (json_t const* accessor = json_getChild(json_getProperty(parent, "accessors")); accessor; accessor = json_getSibling(accessor)) {
        int viewIndex = atoi(json_getPropertyValue(accessor, "bufferView"));
        bufferViews[viewIndex].componentType = json_getInteger(json_getProperty(accessor, "componentType"));
        bufferViews[viewIndex].count = json_getInteger(json_getProperty(accessor, "count"));
        const char *viewType = json_getPropertyValue(accessor, "type");
        if (!strcmp(viewType, "VEC3")) bufferViews[viewIndex].type = GL_FLOAT_VEC3;
        else if (!strcmp(viewType, "VEC2")) bufferViews[viewIndex].type = GL_FLOAT_VEC2;
        else if (!strcmp(viewType, "SCALAR")) bufferViews[viewIndex].type = GL_UNSIGNED_SHORT;
    }
    //printf("accessors\n");

    
    int bvIterator = 0;
    for (json_t const* bv = json_getChild(json_getProperty(parent, "bufferViews")); bv && bvIterator < bufferViewAmount; bv = json_getSibling(bv)) {
        bufferViews[bvIterator].buffer = json_getInteger(json_getProperty(bv, "buffer"));
        bufferViews[bvIterator].byteLength = json_getInteger(json_getProperty(bv, "byteLength"));
        bufferViews[bvIterator].byteOffset = json_getInteger(json_getProperty(bv, "byteOffset"));
        bvIterator++;
    }
    //printf("bufferViews\n");
    
    dest->size = bufferViews[locIndices].count;

    glGenVertexArrays(1, &dest->vao);
    glBindVertexArray(dest->vao);
    glGenBuffers(1, &dest->vbo);
    glGenBuffers(1, &dest->ebo);
    //printf("gen buffers\n");

    json_t const* buffer = json_getChild(json_getProperty(parent, "buffers"));
    //printf("got buffer child json\n");
    const char *bufferEncoded = json_getPropertyValue(buffer, "uri") + 37;
    //printf("got buffer value\n");
    size_t bufferEncodedLength = strlen(bufferEncoded);
    int bufferDecodedLength;
    unsigned char *bufferData = base64Decode(bufferEncodedLength, (unsigned char*)bufferEncoded, &bufferDecodedLength); // 37 gets rid of mime type
    glBindBuffer(GL_ARRAY_BUFFER, dest->vbo);
    glBufferData(GL_ARRAY_BUFFER, bufferDecodedLength, bufferData, GL_STATIC_DRAW);
    //printf("decoded\n");

    for (int i = 0; i < bvIterator; i++) {
        size_t byteLength = bufferViews[i].byteLength;
        size_t byteOffset = bufferViews[i].byteOffset;
        unsigned char *bufferViewData = malloc(byteLength + 1);
        memcpy(bufferViewData, bufferData + byteOffset, byteLength);
        bufferViewData[byteLength] = '\0';

        if (i == locPos) {
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)byteOffset);
            glEnableVertexAttribArray(0);
        } else if (i == locTexCoord) {
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)byteOffset);
            glEnableVertexAttribArray(1);
        } else if (i == locNormal) {
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)byteOffset);
            glEnableVertexAttribArray(2);
        } else if (i == locIndices) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dest->ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, byteLength, bufferViewData, GL_STATIC_DRAW);
        }

        free(bufferViewData);
    }
    glBindVertexArray(0);
    //printf("buffers\n");

    dest->modelFormat = MODEL_GLTF;
    free(bufferData);
    free(modelFileData);
}

void modelLoad(modelinfo *dest, const char *modelPath) {
    if (strstr(modelPath, ".obj")) modelLoadOBJ(dest, modelPath);
    else if (strstr(modelPath, ".gltf")) modelLoadGLTF(dest, modelPath);
}

void modelLoadTextured(modelinfo *dest, const char *modelPath, const char *texPath) {
    if (strstr(modelPath, ".obj")) {
        modelLoadOBJ(dest, modelPath);
        textureLoad(&dest->texture, texPath, 0, 1);
    }
    else if (strstr(modelPath, ".gltf")) {
        modelLoadGLTF(dest, modelPath);
        textureLoad(&dest->texture, texPath, 0, 0);
    }
}

void createFramebuffer(framebuffer *fb, int width, int height, int offsetX) {
    glGenFramebuffers(1, &fb->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
    glGenTextures(1, &fb->tex);
    glBindTexture(GL_TEXTURE_2D, fb->tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->tex, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenRenderbuffers(1, &fb->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, fb->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float right = (float)width/ww;
    float top = (float)height/wh;
    float ox = (float)offsetX/ww;
    float rect[] = {
        -right + ox,  top, 0.0f,  0.0f, 1.0f,
        -right + ox, -top, 0.0f,  0.0f, 0.0f,
        right + ox, -top, 0.0f,  1.0f, 0.0f,
        -right + ox,  top, 0.0f,  0.0f, 1.0f,
        right + ox, -top, 0.0f,  1.0f, 0.0f,
        right + ox,  top, 0.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &fb->vao);
    glBindVertexArray(fb->vao);
    glGenBuffers(1, &fb->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, fb->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    fb->shader = shaderLoad("assets/shaders/framebuffer.vs", "assets/shaders/framebuffer.fs");

    glUseProgram(fb->shader);
    glUniform1i(glGetUniformLocation(fb->shader, "tex"), 0);
}

void loadIntoVAO(modelinfo *model) {
    glGenVertexArrays(1, &model->vao);
    glBindVertexArray(model->vao);
    glGenBuffers(1, &model->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, model->size*8*sizeof(float), model->verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    free(model->verts);
}

void loadUniformsAndDraw(modelinfo mdl, unsigned int shader) {
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, view);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, projection);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, model);
    glUniform3fv(glGetUniformLocation(shader, "lightPos"), 1, lightPos);
    glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, cameraPos);
    if (mdl.texture.tex) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mdl.texture.tex);
    }
    glBindVertexArray(mdl.vao);
    glDrawArrays(GL_TRIANGLES, 0, mdl.size);
}

void loadUniformsAndDrawIndexed(modelinfo mdl, unsigned int shader) {
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, view);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, projection);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, model);
    glUniform3fv(glGetUniformLocation(shader, "lightPos"), 1, lightPos);
    glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, cameraPos);
    if (mdl.texture.tex) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mdl.texture.tex);
    }
    glBindVertexArray(mdl.vao);
    glDrawElements(GL_TRIANGLES, mdl.size, GL_UNSIGNED_SHORT, 0);
}

void createRectFromTexture(modelinfo *mdl, textureinfo texture) {
    float *verts = malloc(48*sizeof(float));
    verts[0] = verts[24] = verts[32] = (float)texture.width;
    verts[9] = verts[33] = verts[41] = (float)texture.height;
    verts[1] = verts[2] = verts[4] = verts[5] = verts[6] = verts[8] = verts[10] = verts[11] = verts[13] = verts[14] = verts[16] = verts[17] = verts[18] = verts[19] = verts[20] = verts[21] = verts[22] = verts[25] = verts[26] = verts[28] = verts[29] = verts[30] = verts[34] = verts[37] = verts[38] = verts[40] = verts[42] = verts[43] = verts[45] = verts[46] = 0.0;
    verts[3] = verts[7] = verts[12] = verts[15] = verts[23] = verts[27] = verts[31] = verts[35] = verts[36] = verts[39] = verts[44] = verts[47] = 1.0;
    mdl->verts = verts;
    mdl->size = 6;
}

void orthoMode(void) {
    memcpy(projection, projOrtho, sizeof(mat4f));
    vec3f zeroPos = {0.0, 0.0, 3.0};
    vec3f flatFront = {0.0, 0.0, -1.0};
    vec3f flatUp = {0.0, 1.0, 0.0};
    vec3f viewCenter;
    vec3f_add(viewCenter, zeroPos, flatFront);
    mat4f_lookat(view, zeroPos, viewCenter, flatUp);
}