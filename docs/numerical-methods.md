## Runge-Kutta 4 (RK4) Integration — Explained

When we talk about geodesics in curved spacetime, we’re dealing with a system of differential equations that describe how a particle—or in our case, a ray of light—moves. These equations are usually too complicated to solve exactly, so we turn to numerical methods. One of the most popular choices is the **fourth-order Runge-Kutta method (RK4)**.

Think of RK4 like taking careful steps along a winding mountain trail. At each step, instead of just looking straight ahead, RK4 takes a few “sneak peeks” along the way to estimate the path more accurately.

### The Idea in Simple Terms

Suppose you know where you are at a particular moment and you know the slope of your path (the derivative). A naive method like **Euler’s method** would take a single step using that slope and call it a day. But if the slope changes a lot along your step, Euler can easily go off-track.

RK4 improves on this by taking **four evaluations** of the slope at carefully chosen points:

1. **Start of the step** — check the slope right where you are (`k1`).
2. **Halfway in, using the first slope** — imagine taking a mid-step to see if the slope changes (`k2`).
3. **Halfway in, using the second slope** — another mid-step with a slightly better estimate (`k3`).
4. **End of the step** — take a peek at the slope at the far end of your step (`k4`).

Then RK4 combines all these slopes in a weighted average:

$$
y_{n+1} = y_n + \frac{h}{6} (k_1 + 2 k_2 + 2 k_3 + k_4)
$$

This weighted combination gives a very accurate estimate of where you should be at the next step.

### Why RK4 Works Well for Geodesics

In the context of geodesics:

* Each ray has six “pieces of information”: position `(r, θ, φ)` and velocity `(dr/dλ, dθ/dλ, dφ/dλ)`.
* The RK4 method allows us to update all six components **simultaneously**, while keeping the accumulated error small.
* Because spacetime curvature can change dramatically near a black hole, RK4 is especially helpful: it’s stable enough to handle strong curvature without requiring tiny steps everywhere.

### A Visual Analogy

Imagine you’re rowing a boat down a twisting river:

* **Euler**: You look at the current direction, row a fixed distance, and hope for the best. You’ll likely drift off course if the river bends sharply.
* **RK4**: You peek ahead four times along your intended path and adjust your stroke accordingly. You stay much closer to the true river path, even around tight bends.

### Accuracy

* RK4 is called **fourth-order** because the error per step scales with $h^5$, and the total accumulated error scales roughly with $h^4$ (where $h$ is your step size).
* This means you can take reasonably large steps without losing accuracy, which is critical when simulating millions of rays efficiently.

### Connecting to Code

In your code, each `k` evaluation corresponds to calculating how the ray’s position and velocity would change at different “guesses” along the step. The final weighted combination moves the ray forward accurately in spacetime.

By using RK4, we’re essentially giving each ray a **very careful and informed nudge**, instead of blindly pushing it along, which is why the results are both stable and accurate—even near the extreme curvature of a black hole.