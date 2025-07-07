import re

def count_nodes(expression):
    # Remove all parentheses
    expr_clean = re.sub(r'[()]', ' ', expression)

    # Split by spaces
    tokens = expr_clean.split()

    # Filter out empty strings and count all remaining tokens
    nodes = [token for token in tokens if token.strip() != '']
    
    return len(nodes)

data = [
    [
        "((t + ((q + t) max (wt * t_rem))) * ((d + (t_rem / dt)) * ((dem - d) max (d max slack_t))))",
        "(((dt + (slack_t / dem)) + ((wt + d) * (d max t_rem))) + (((dem / t_ready) - (t_ready + t_rem)) min (dt + (d_depot max dt))))",
        "(((q max (rt - t_rem)) - next_veh) max (((d max wt) max (slack_t / q)) * ((wt + wt) + (slack_t max d))))",
        "((((t_rem max d) min (d + t_ready)) + ((d - next_veh) + (dt + wt))) + ((d min (t_ready max t)) max ((next_veh / t) max (t_ready / d_depot))))",
        "((d + ((d - t_ready) + (d max d))) + (((wt + d) min (t_rem + next_veh)) * ((slack_t max d) + (d / d_depot))))",
        "(((wt * slack_t) max (t_rem + (d + d))) * ((slack_t / (rt - t)) max ((t_rem + d) - (next_veh - d))))",
        "((((q / d_depot) max (d + t_rem)) * (wt + (wt + d))) * (((t_rem - next_veh) + d) max ((t_rem / next_veh) * (wt max d))))",
        "((((wt + d) * t_rem) max (t / (next_veh / d))) + (((wt max dt) max (wt + t_ready)) max ((slack_t min slack_t) + next_veh)))",
        "((((dt + wt) + d) - dt) * (((d_depot / t_ready) max (slack_t + slack_t)) max ((d + slack_t) + d)))",
        "((((dt - q) min (q - q)) + wt) + (((d max d) / (next_veh / slack_t)) + (d + wt)))"
    ],
    [
        "(d + (((dt - t_rem) max ((dt max t_ready) max (t_rem + t_ready))) max (((d - t_rem) + (t + dt)) * ((t_ready * slack_t) / (slack_t * q)))))",
        "((dt + (((d max d) * (d max slack_t)) + ((d - next_veh) + (wt * slack_t)))) + ((((d min rt) + (rt + d_depot)) * ((dem * dt) min (t_rem / next_veh))) max d))",
        "(((((slack_t * d) min t_ready) + ((q max dt) max (d * d))) max (dem max ((d + wt) * (slack_t + wt)))) * (((dt / dem) * (t_rem / (dt / d))) + q))",
        "(((((t + t) / q) max ((slack_t max t_rem) + (wt min q))) min (((dem + dem) + (q max d)) / ((t max t_ready) / (rt max t_ready)))) + (((d + (wt + t)) - ((d * slack_t) min next_veh)) + d))",
        "(wt + ((((t_rem + t_rem) + dt) * d) / (((q max dem) - wt) max (rt - t_rem))))",
        "((((t_rem * (wt + d)) max (q / (t_ready * wt))) + next_veh) + dt)",
        "(((dt max ((wt - d_depot) * t)) max (((wt min slack_t) + (t_rem max d_depot)) - t)) + (d + (wt max (d / next_veh))))",
        "(((((next_veh + slack_t) / (t_ready min t_rem)) / wt) max (((d - t_ready) + (rt - wt)) max ((t max dt) max d))) + (((wt + (dt min d)) * t_rem) - (d_depot min (slack_t * (dem / dt)))))",
        "((dem max (((d * t) max (t - rt)) - ((dem max wt) - (t_ready - rt)))) + (((slack_t - next_veh) * d_depot) max ((wt max (d + wt)) * ((d min rt) + (wt + slack_t)))))",
        "(((((next_veh / d_depot) / dem) max next_veh) + t_rem) max ((((slack_t + d) + t) * ((t_ready max q) + (d min t))) max (((slack_t / next_veh) min wt) * (t_ready * (dt max rt)))))"
    ],
    [
        "(wt + (((d_depot / (d min d)) * ((t / q) - (slack_t min slack_t))) * (((q + q) max (dt - dem)) / ((t_rem * slack_t) max (dt / q)))))",
        "((wt max ((q min t) / ((dt + q) / (q min slack_t)))) + ((((d_depot + t_rem) / (q + d_depot)) max ((t min wt) - (t_rem min dem))) max (((wt min dem) min dt) max d)))",
        "(((((rt min dt) * q) / t_rem) - ((t_rem / (q - dem)) max ((rt * q) / (wt max d)))) / ((((dt * d) + (d - rt)) * ((wt / rt) + (t_rem * wt))) max (rt * (t_rem max (slack_t * q)))))",
        "((((rt + (d - t_rem)) min (d - (d_depot * slack_t))) / (((d / q) min (rt / slack_t)) min ((t_rem + dt) min (q / d_depot)))) - ((((t_rem min wt) - (d - t_ready)) - (d max (d_depot / t_rem))) - (((d_depot min wt) - (t_rem / t_ready)) * ((slack_t * wt) * (wt min wt)))))",
        "(((((t_rem min wt) min (slack_t - t)) - ((q / d) min (dem max d))) max (d - t_rem)) max ((rt + (t_ready / dem)) min ((d - (slack_t max q)) / ((d max wt) * t_rem))))",
        "(((((d / rt) / (d_depot * t)) min ((d_depot max slack_t) / t_rem)) / ((q / (dt * d)) + wt)) - (((dem * (t / rt)) / ((d * rt) * d)) min (((d_depot - t_rem) * (wt max t_ready)) min ((t_rem - d) + (dem / t)))))",
        "(((((dt - t_ready) / slack_t) max ((t - slack_t) + (dt - t_ready))) / ((d_depot / (d * dem)) + ((slack_t min dem) * (t max d)))) + ((wt * ((t_rem max t_ready) min wt)) max (((dt * t_rem) / (d_depot / d)) / ((dt min dt) + (slack_t min t_ready)))))",
        "(((wt * (wt min (t_rem - wt))) max d) max ((t_rem min ((rt min t_rem) - (d + wt))) * (((slack_t min rt) - rt) - d)))",
        "(((((rt / d) + rt) * ((wt - dem) - t)) max (((wt max d) max (d - t_rem)) max ((t_rem / wt) / (d - wt)))) * ((slack_t / q) / ((q + (dem / wt)) min ((t_rem - t_rem) max (d_depot min slack_t)))))",
        "((((d / slack_t) * t) min d) + (((t max (dt + rt)) min wt) max (((dem - t_rem) max (d + d)) - ((wt - d) + (rt * slack_t)))))"
    ]
]

total_nodes = []
for method in range(len(data)):
    list = []
    for i in range(len(data[method])):
        list.append(count_nodes(data[method][i]))
    total_nodes.append(list)

print (total_nodes)
