#ifndef BVH_H
#define BVH_H

#include <vector>
#include <cstdlib>
#include <chrono>
#include "../include/mesh.hcu"

struct AABB
{
    glm::vec3 aabbMin;
    glm::vec3 aabbMax;
    AABB() : aabbMin(glm::vec3(1e30f)), aabbMax(glm::vec3(-1e30f)) {};
    float area() 
    {
        glm::vec3 diff = aabbMax - aabbMin;
        return diff.x*diff.y + diff.x*diff.z + diff.y*diff.z;
    };
    void merge(glm::vec3 p)
    {   
        for(int i = 0; i<3; ++i)
        {
            aabbMin[i] = fminf(aabbMin[i], p[i]);
            aabbMax[i] = fmaxf(aabbMax[i], p[i]);
        }
    };
};

struct Node
{
    AABB aabb;
    int triCount;
    // if triCount = 0 => BVHNode != leafNode => leftIdx == Left node index
    // if triCount != 0 => BVHNode == leafNode => leftIdx == index of the first triangle 
    // (ORIGINAL INDEX -> index in the original list of triangles)
    int leftIdx; 
};

struct Triangle
{
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 p2;
    glm::vec3 centroid;
    glm::vec3 normal0; // normal to the triangle's face
    glm::vec3 normal1; // normal to the triangle's face
    glm::vec3 normal2; // normal to the triangle's face
    glm::vec3 normal;

    Triangle(GLuint _p0, GLuint _p1, GLuint _p2, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals) : p0(vertices[_p0]), p1(vertices[_p1]), p2(vertices[_p2])
    {

        centroid = (p0 + p1 + p2)/3.0f;
        normal0 = normals[_p0];
        normal1 = normals[_p1];
        normal2 = normals[_p2];
 
        normal = cross(p1 - p0, p2 - p0); 
    }
};

class BVH
{
    public:
    BVH(Mesh *mesh) : m_NTri(mesh->getIndicesNb()/3), m_nodesNb(0)
    {
        /**
         * Builds a BVH structure corresponding to the given mesh
        */
       auto start = std::chrono::steady_clock::now();
       int nbIndices = mesh->getIndicesNb();
       int nbTri = nbIndices/3;

       std::vector<glm::vec3> &vertices = mesh->getVertices();
       std::vector<GLuint> &indices = mesh->getIndices();
       std::vector<glm::vec3> &normals = mesh->getNormals();

        // allocating memory for node pool
        int nbMaxNodes = 2 * nbTri;
        std::cout << "size tree = " << nbMaxNodes << std::endl;
        std::cout << "nb Tri mesh = " << nbTri << std::endl;
        tree = new Node[nbMaxNodes];


        // initialize triangle indices && triangle structs
        for (int idx=0; idx<nbIndices; idx+=3)
        {
            m_triIndices.push_back(idx/3);
            m_triangles.push_back(Triangle(indices[idx], indices[idx+1], indices[idx+2], vertices, normals));
        }

        // root node info
        Node &root = tree[0];
        root.leftIdx=0; 
        root.triCount=nbTri;
        m_nodesNb++;

        /* Code à évaluer */
        buildBV(0);

        // building recursively the BVH starting from root node
        buildTreeRec(0);
        
        std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() -start;
        std::cout << "BVH built in " << elapsed_seconds.count() << "s -------------------- \n\n";

        // printTree(0);

    }

    void printTree(int idx)
    {
        Node &node = tree[idx];
        printNode(node, idx);

        int leftNode = node.leftIdx;
        if (tree[leftNode].triCount == 0)
        {
            printTree(leftNode);
        }
        else
        {
            printTriangles(tree[leftNode], leftNode);
        }
        int rightNode = node.leftIdx + 1;
        if (rightNode >= m_nodesNb) return;
        if (tree[rightNode].triCount == 0)
        {
            printTree(rightNode);
        }
        else
        {
            printTriangles(tree[rightNode], rightNode);
        }
}
    
    ~BVH()
    {
        delete tree;
    }


    Node *getTree() {return tree;};
    GLuint *triIndices() {return m_triIndices.data();};

    Triangle *tri() {return m_triangles.data();};

    int getNbNodes() {return m_nodesNb;};
    int getNbTri() {return m_triangles.size();};
    int getNbtriIdx() {return m_triIndices.size();};

    private:

    int m_NTri; // number of triangles in mesh
    int m_nodesNb;
    std::vector<GLuint> m_triIndices; //
    std::vector<Triangle> m_triangles;


    Node *tree;

    void printTriangles(const Node &node, int index)
    {
        printf("PRINTING LEAF NODE at index = %i && with tri count = %i <<<<<\n", index, node.triCount);
        for (int i =0; i<node.triCount; ++i)
        {
            Triangle &tri = m_triangles[m_triIndices[i+node.leftIdx]];
            printf("\nTriangle at centroid = %f %f %f && index = %i", tri.centroid.x, tri.centroid.y, tri.centroid.z, m_triIndices[i+node.leftIdx]); 
        }
        std::cout << std::endl;

    }
    void printNode(const Node &node, int idx)
    {
        std::cout << "NODE at index = " << idx << " //  leftFirst  = " << node.leftIdx << std::endl;
    }

    void buildTreeRec(int currentIdx)
    {
        Node &node = tree[currentIdx];
        // computing best position & axis for the AABB's split using SAH
        int splitAxis;
        float splitPos;
        float minCost = 1e30f; // the best split is the one that minimizes all intersections <!>

        float currentPos;
        float currentCost;

        if (node.triCount <= 2) return; // don't continue if number of triangles is inferior to 2

        for (int axis = 0; axis<3; ++axis)
        {
            float AABBmin = node.aabb.aabbMin[axis];
            float AABBmax = node.aabb.aabbMax[axis];
            float step = (AABBmax - AABBmin)/89.0;

            for (int k = 0; k<90; ++k)
            {

                // testing one axis per triangle in the current node
                currentPos = AABBmin + k * step;
                currentCost = evaluateCost(node, axis, currentPos);
                if (currentCost < minCost)
                {
                    splitPos = currentPos;
                    minCost = currentCost;
                    splitAxis = axis;
                }
            }
        }
        
        // if the current node's cost is less than best cost, then we can stop subdiving the tree here
        float currNodeCost = node.aabb.area() * node.triCount;  
        if (currNodeCost <= minCost) return;

        int triIdx = node.leftIdx;
        int lastTriIdx = triIdx + node.triCount - 1;

        // we sort triangle indices so that left triangles are below the split line
        while (triIdx <= lastTriIdx)
        {
            if (m_triangles[m_triIndices[triIdx]].centroid[splitAxis] < splitPos) triIdx++;
            else 
            {
                // quick sort (using indices instead of copying triangles)
                int temp = m_triIndices[triIdx];
                m_triIndices[triIdx] = m_triIndices[lastTriIdx];
                m_triIndices[lastTriIdx--] = temp; 
            }
            
        }
        // std::cout << "left first idx = " << triIdx << " && last  tri idx = " << lastTriIdx;

        int leftTriCount = triIdx - node.leftIdx;
        if (leftTriCount == 0 || leftTriCount == node.triCount) return;  // if one of the sides is empty we do not split
    
        // <!>  at this stage of the function, we know the current node is NOT a leaf node.

        int leftChild = m_nodesNb++;
        int rightChild = m_nodesNb++;

        tree[leftChild].leftIdx = node.leftIdx;
        tree[leftChild].triCount = leftTriCount;

        tree[rightChild].leftIdx = triIdx;
        tree[rightChild].triCount = node.triCount - leftTriCount;


        buildBV(leftChild);
        buildBV(rightChild);

        // -> current node possesses at least 1 child with a number of triangles != 0
        node.leftIdx = leftChild;
        node.triCount = 0;

        buildTreeRec(leftChild);
        buildTreeRec(rightChild);
    };

    float evaluateCost(Node &node, int &axis, float &splitPos) 
    {
        AABB lBox, rBox;
        int lTriNb = 0;
        int rTriNb = 0;

        for (int i=0; i<node.triCount; ++i)
        {
            // computing left box cost
            Triangle &tri = m_triangles[m_triIndices[node.leftIdx+i]];

            if (tri.centroid[axis] < splitPos)
            {
                // merge left box with current triangle
                lBox.merge(tri.p0);
                lBox.merge(tri.p1);
                lBox.merge(tri.p2);
                lTriNb++;
            }
            else
            {
                // merge right box with current triangle
                rBox.merge(tri.p0);
                rBox.merge(tri.p1);
                rBox.merge(tri.p2);
                rTriNb++;
            }
        }
        float cost = lTriNb * lBox.area() + rTriNb * rBox.area();
        return cost > 0.0 ? cost : 1e30f;
    };

    void buildBV(int index)
    {
        /**
         * Build the bounding volume (here: AABB) of the node at given index.
        */

        Node& node = tree[index];
        for (int i = 0; i < node.triCount; i++)
        {
            Triangle& leafTri = m_triangles[m_triIndices[node.leftIdx + i]];
            node.aabb.merge(leafTri.p0);
            node.aabb.merge(leafTri.p1);
            node.aabb.merge(leafTri.p2);
        }
    }
};


#endif