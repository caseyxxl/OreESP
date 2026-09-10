#include "main.h"
#include <cmath>

static bool WorldToScreen(const float mv[16], const float pr[16], const int vp[4],
                          double wx, double wy, double wz, float& sx, float& sy)
{
    double v[4] = { wx, wy, wz, 1.0 };
    double m[4], p[4];

    for (int i = 0; i < 4; i++)
        m[i] = mv[0*4+i]*v[0] + mv[1*4+i]*v[1] + mv[2*4+i]*v[2] + mv[3*4+i]*v[3];
    for (int i = 0; i < 4; i++)
        p[i] = pr[0*4+i]*m[0] + pr[1*4+i]*m[1] + pr[2*4+i]*m[2] + pr[3*4+i]*m[3];

    if (p[3] <= 0.0) return false;

    double ndcX = p[0] / p[3];
    double ndcY = p[1] / p[3];

    sx = vp[0] + (vp[2] * (float)(ndcX + 1.0) * 0.5f);
    sy = vp[1] + (vp[3] * (float)(1.0 - ndcY) * 0.5f);
    return true;
}

void DrawESP() {
    if (!gladLoadGL()) return;

    float mv[16], pr[16];
    int   vp[4];
    glGetFloatv(GL_MODELVIEW_MATRIX, mv);
    glGetFloatv(GL_PROJECTION_MATRIX, pr);
    glGetIntegerv(GL_VIEWPORT, vp);

    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, vp[2], vp[3], 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLineWidth(1.5f);
    glBegin(GL_LINES);

    std::lock_guard<std::mutex> lk(g_oresMutex);
    for (const auto& o : g_ores) {
        switch (o.id) {
            case 56:  glColor4f(0, 1, 1, 0.9f);     break;
            case 14:  glColor4f(1, 0.85f, 0, 0.9f); break;
            case 15:  glColor4f(0.8f, 0.6f, 0.4f, 0.9f); break;
            case 16:  glColor4f(0.1f, 0.1f, 0.1f, 0.9f); break;
            case 21:  glColor4f(0.2f, 0.3f, 0.9f, 0.9f); break;
            case 73:  glColor4f(1, 0.1f, 0.1f, 0.9f); break;
            case 129: glColor4f(0, 1, 0.4f, 0.9f);  break;
            default:  glColor4f(1, 1, 1, 0.9f);
        }

        double c[8][3] = {
            {o.x,o.y,o.z},{o.x+1,o.y,o.z},{o.x+1,o.y+1,o.z},{o.x,o.y+1,o.z},
            {o.x,o.y,o.z+1},{o.x+1,o.y,o.z+1},{o.x+1,o.y+1,o.z+1},{o.x,o.y+1,o.z+1}
        };
        int e[12][2] = {{0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}};

        float s[8][2]; bool ok[8];
        for (int i = 0; i < 8; i++)
            ok[i] = WorldToScreen(mv, pr, vp, c[i][0], c[i][1], c[i][2], s[i][0], s[i][1]);

        for (auto& edge : e) {
            if (!ok[edge[0]] || !ok[edge[1]]) continue;
            glVertex2f(s[edge[0]][0], s[edge[0]][1]);
            glVertex2f(s[edge[1]][0], s[edge[1]][1]);
        }
    }

    glEnd();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
