# Mathematical Theory of Black Hole Geodesics

### Einstein’s Field Equations

General Relativity describes gravity not as a force, but as the curvature of spacetime caused by mass and energy. This curvature is captured by **Einstein’s field equations**:

$$
G_{\mu\nu} = \frac{8\pi G}{c^4} T_{\mu\nu}
$$

Here:

* $G_{\mu\nu}$ is the **Einstein tensor**, describing spacetime curvature.
* $T_{\mu\nu}$ is the **stress-energy tensor**, representing matter and energy.
* $G$ is Newton’s gravitational constant, and $c$ is the speed of light.

### Spacetime Metric

Distances in spacetime are described using the **metric tensor** $g_{\mu\nu}$:

$$
ds^2 = g_{\mu\nu} dx^\mu dx^\nu
$$

where $ds^2$ is the spacetime interval between two events.

## Schwarzschild Metric

For a **spherically symmetric, non-rotating mass** (like a static black hole), the Schwarzschild solution gives the spacetime geometry:

$$
ds^2 = -\left(1 - \frac{2GM}{c^2 r}\right) dt^2 + \left(1 - \frac{2GM}{c^2 r}\right)^{-1} dr^2 + r^2 (d\theta^2 + \sin^2\theta \, d\phi^2)
$$

* $M$ = mass of the black hole
* $r, \theta, \phi$ = spherical coordinates
* $t$ = time coordinate

The **Schwarzschild radius** $r_s$ marks the event horizon:

$$
r_s = \frac{2GM}{c^2}
$$

Inside $r_s$, not even light can escape.

We often write the metric using the **lapse function** $f(r)$:

$$
ds^2 = -f(r) dt^2 + f(r)^{-1} dr^2 + r^2 (d\theta^2 + \sin^2\theta \, d\phi^2), \quad f(r) = 1 - \frac{r_s}{r}
$$

## Geodesics: Paths of Free-Falling Particles and Light

A **geodesic** is the path that a particle follows when moving under gravity alone. For light rays, $ds^2 = 0$ (null geodesics).

### Lagrangian Formulation

We can derive the geodesic equations from a Lagrangian:

$$
L = \frac{1}{2} g_{\mu\nu} \dot{x}^\mu \dot{x}^\nu, \quad \dot{x}^\mu = \frac{dx^\mu}{d\lambda}
$$

where $\lambda$ is an affine parameter along the geodesic.

### Conserved Quantities

Because the Schwarzschild metric is **time-independent** and **spherically symmetric**, we have two key conserved quantities:

1. **Energy** (from time translation symmetry):

$$
E = - g_{tt} \frac{dt}{d\lambda} = f(r) \frac{dt}{d\lambda}
$$

2. **Angular Momentum** (from rotational symmetry):

$$
L = g_{\phi\phi} \frac{d\phi}{d\lambda} = r^2 \sin^2 \theta \frac{d\phi}{d\lambda}
$$

### Derivation of the Geodesic Equations

Geodesics satisfy the **Euler-Lagrange equations**:

$$
\frac{d}{d\lambda} \left(\frac{\partial L}{\partial \dot{x}^\mu}\right) - \frac{\partial L}{\partial x^\mu} = 0
$$

#### 1. Radial Motion

For the Schwarzschild metric:

$$
L = \frac{1}{2} \left[-f(r) \dot{t}^2 + f(r)^{-1} \dot{r}^2 + r^2 (\dot{\theta}^2 + \sin^2\theta \, \dot{\phi}^2)\right]
$$

The radial Euler-Lagrange equation becomes:

$$
\ddot{r} = -\frac{GM}{r^2} (\dot{t})^2 + \frac{GM}{r^2 f(r)} (\dot{r})^2 + r f(r) \left(\dot{\theta}^2 + \sin^2\theta \, \dot{\phi}^2\right)
$$

#### 2. Angular Motion

$$
\ddot{\theta} = -\frac{2}{r} \dot{r} \dot{\theta} + \sin\theta \cos\theta \, \dot{\phi}^2
$$

$$
\ddot{\phi} = -\frac{2}{r} \dot{r} \dot{\phi} - 2 \cot\theta \, \dot{\theta} \dot{\phi}
$$

Here, $\dot{}$ denotes derivative with respect to $\lambda$.

These equations fully describe how light or particles move around a Schwarzschild black hole.

### Numerical Implementation

In a shader or simulation, we integrate these equations using:

```glsl
void GeodesicRHS(Ray ray, out vec3 d1, out vec3 d2) 
{
    float r      = ray.r;
    float theta  = ray.theta;
    float dr     = ray.dr;
    float dtheta = ray.dtheta;
    float dphi   = ray.dphi;
    float f      = 1.0 - SagA_rs / r;
    float dt_dL  = ray.E / f;

    d1 = vec3(dr, dtheta, dphi);
    d2.x = - (SagA_rs / (2.0 * r*r)) * f * dt_dL * dt_dL
           + (SagA_rs / (2.0 * r*r * f)) * dr * dr
           + r * (dtheta*dtheta + sin(theta)*sin(theta)*dphi*dphi);
    d2.y = -2.0*dr*dtheta/r + sin(theta)*cos(theta)*dphi*dphi;
    d2.z = -2.0*dr*dphi/r - 2.0*cos(theta)/(sin(theta)) * dtheta * dphi;
}
```

### Conserved Quantities in Code

```glsl
ray.E = f * dt_dL;                   // Energy
ray.L = ray.r * ray.r * sin(ray.theta) * ray.dphi; // Angular momentum
```

### Effective Potential

The **radial motion** can be described using an effective potential:

$$
V_\text{eff}(r) = \left(1 - \frac{r_s}{r}\right) \frac{L^2}{r^2}
$$

This potential defines the possible orbits of light or particles.

### Relativistic Effects Around Black Holes

* **Gravitational Lensing:** Light bends around the black hole, producing Einstein rings, multiple images, or distorted images.
* **Event Horizon:** Located at $r = r_s$, where nothing escapes.
* **Photon Sphere:** At $r = 1.5 r_s$, light can orbit in unstable circular paths.

### Numerical Considerations

* **Event Horizon:** Integration becomes singular at $r = r_s$. Use adaptive step sizes or terminate integration near the horizon.
* **Coordinate Poles:** Spherical coordinates have singularities at $\theta = 0, \pi$. Avoid direct integration through these points or use transformations.
