// ============================================================
//  Chassi do carrinho-robo - V0.2
//  Project-based Maker Lab (FIAP)
//  Ferramenta: OpenSCAD
//
//  Como exportar o STL:
//    1. abrir este arquivo no OpenSCAD
//    2. F6 (render) e depois F7 (export STL)
//    3. salvar em /cad/stl/chassi_v0.2.stl
// ============================================================

/* [Placa principal] */
comprimento   = 200;   // mm
largura       = 150;   // mm
espessura     = 4;     // mm
raio_canto    = 15;    // mm

/* [Motores] */
dist_eixo_frente = 70;   // distancia da frente ate o centro do eixo
larg_motor       = 23;   // largura do motor amarelo TT
comp_motor       = 38;
folga_roda       = 12;   // recorte lateral pra roda passar

/* [Furacao] */
furo_m3       = 3.4;    // passagem folgada pra parafuso M3
furo_abracadeira_l = 8; // rasgo pra abracadeira (enforca-gato)
furo_abracadeira_a = 3;
$fn = 48;

// ------------------------------------------------------------
module placa_base() {
    hull() {
        for (x = [raio_canto, comprimento - raio_canto],
             y = [raio_canto, largura - raio_canto])
            translate([x, y, 0]) cylinder(h = espessura, r = raio_canto);
    }
}

module recortes_roda() {
    for (y = [-1, largura - folga_roda + 1])
        translate([dist_eixo_frente - comp_motor/2, y, -1])
            cube([comp_motor, folga_roda, espessura + 2]);
}

module furos_motor() {
    // pares de furos M3 pra prender as braadeiras/suportes dos motores TT
    for (y = [folga_roda + 6, largura - folga_roda - 6])
        for (dx = [-14, 14])
            translate([dist_eixo_frente + dx, y, -1])
                cylinder(h = espessura + 2, d = furo_m3);
}

module furos_eletronica() {
    // grade de furos M3 a cada 15 mm - permite reposicionar Arduino, L298N e regulador
    for (x = [60 : 15 : comprimento - 40])
        for (y = [30 : 15 : largura - 30])
            translate([x, y, -1]) cylinder(h = espessura + 2, d = furo_m3);
}

module furos_bateria() {
    // fixacao do suporte 2x18650, centralizado e atras do eixo
    for (dx = [-32, 32])
        for (dy = [-22, 22])
            translate([comprimento - 60 + dx, largura/2 + dy, -1])
                cylinder(h = espessura + 2, d = furo_m3);
}

module rasgos_abracadeira() {
    // canaletas laterais: esquerda = alimentacao, direita = sinal
    for (y = [14, largura - 14])
        for (x = [40 : 25 : comprimento - 30])
            translate([x, y - furo_abracadeira_a/2, -1])
                cube([furo_abracadeira_l, furo_abracadeira_a, espessura + 2]);
}

module suporte_sensor() {
    // aba frontal vertical pro HC-SR04 (dois furos de 16 mm de centro a centro)
    translate([6, largura/2 - 25, espessura])
        difference() {
            cube([4, 50, 22]);
            for (dy = [11, 39])
                translate([-1, dy, 12]) rotate([0, 90, 0]) cylinder(h = 6, d = 16.2);
        }
}

module furo_roda_boba() {
    for (dx = [-10, 10])
        translate([comprimento - 25 + dx, largura/2, -1])
            cylinder(h = espessura + 2, d = furo_m3);
}

// ------------------------------------------------------------
module chassi() {
    difference() {
        placa_base();
        recortes_roda();
        furos_motor();
        furos_eletronica();
        furos_bateria();
        rasgos_abracadeira();
        furo_roda_boba();
    }
    suporte_sensor();
}

chassi();
