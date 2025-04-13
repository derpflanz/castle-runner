$(() => {
    new PixelArt($("#table"), 320, 40);
});

class PixelArt {
    constructor(table, cols, rows) {
        this.table = table;
        this.cols = cols;
        this.rows = rows;
        this.data = JSON.parse(localStorage.getItem("data"))??{};
        this.initTable(cols, rows);        

        $("#btnSave").on('click', (event) => this.btnSave_onClick(event));
        $("#btnClear").on('click', (event) => this.btnClear_onClick(event));
        $("#btnHex").on('click', (event) => this.btnHex_onClick(event));
        $("#btnCopy").on('click', (event) => this.btnCopy_onClick(event));
    }

    btnCopy_onClick(event) {
        this.btnHex_onClick(event);
        navigator.clipboard.writeText($("#txtResult").text());
    }

    ident(x, y) {
        return `${x}_${y}`;
    }

    btnHex_onClick(event) {
        let result = "";

        for (let col = 0; col < (this.cols / 8); col++) {
            for (let row = 0; row < this.rows; row++) {
                let hex = 0;

                // take 8 bits
                for (let i = 0; i < 8; i++) {
                    let ncol = col * 8 + i;
                    let td = $(`#${this.ident(ncol, row)}`);

                    if (td.hasClass('bg-secondary')) {
                        hex += 1;
                    }

                    if (i < 7) {
                        hex <<= 1;
                    }
                }

                result += hex.toString(16).padStart(2, "0") + " ";
            }
            result += "<br>";
        }

        $("#txtResult").html(result);
    }

    btnClear_onClick(event) {
        $("td").removeClass("bg-secondary");        
    }

    btnSave_onClick(event) {
        let saveData = {}
        for (let td of $("td")) {
            if ($(td).hasClass('bg-secondary')) {
                let d = $(td).data()
                let ident = this.ident(d['x'], d['y']);

                saveData[ident] = true;
            }
        }

        localStorage.setItem("data", JSON.stringify(saveData));
    }

    initTable(cols, rows) {
        this.table.empty();
        let tbody = $("<tbody>").appendTo(this.table);

        for (let y = 0; y < rows; y++) {
            let trow = $("<tr>").appendTo(tbody);
            for (let x = 0; x < cols; x++) {
                let tcell = $(`<td class='border' style='height: 0.5em; min-width: 0.5em;' id='${this.ident(x, y)}'>`);
                tcell.data({ x: x, y: y});
                if (this.data[this.ident(x, y)] == true) {
                    tcell.addClass('bg-secondary');
                }
                tcell.appendTo(trow);
            }
        }

        $("td").on('click', (event) => this.onTd_Click(event));
        $("td").on('mouseover', (event) => this.onTd_MouseOver(event));
    }

    onTd_MouseOver(event) {
        event.originalEvent.buttons==1?this.onTd_Click(event):null;

        let td = $(event.currentTarget);
        let parts = td.attr("id").split("_");

        $("#x").text(parts[0]);
        $("#y").text(parts[1]);
    }

    onTd_Click(event) {
        let td = $(event.currentTarget);

        if (td.hasClass("bg-secondary")) {
            td.removeClass("bg-secondary");
        } else {
            td.addClass("bg-secondary");
        }
    }
}

