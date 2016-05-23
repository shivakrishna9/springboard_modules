(function ($) {
    Drupal.behaviors.fdBundler = {
        attach: function (context, settings) {
            $(window).ready(function () {
                window.App = {
                    Handlers: {}
                };
                App.Handlers.reorderItems = function () {
                    $('#fundraiser-designations-categories table:not("table.sticky-header")').draggable({
                        connectToSortable: "#fundraiser-designations-groups form",
                        helper: "clone",
                        revert: false,
                        placeholder: 'table-sortable-placeholder',
                        opacity: 0.5,
                        delay: 150,
                        cursorAt: {right: 25},
                        stop: function (event, ui) {
                            App.Handlers.reorderItems();
                        }
                    });

                    $(".designations-category-table tr").draggable({
                        connectToSortable: "#fundraiser-designations-groups form table",
                        helper: "clone",
                        revert: false,
                        cancel: 'th',
                        delay: 150,
                        placeholder: 'row-sortable-placeholder',
                        cursorAt: {right: 25},
                        stop: function (event, ui) {
                            App.Handlers.reorderItems();
                        }
                    });

                    $('#fundraiser-designations-groups form table:not("table.sticky-header") tr').draggable({
                        connectToSortable: "#fundraiser-designations-groups table",
                        helper: "original",
                        revert: false,
                        cancel: 'th',
                        placeholder: 'row-sortable-placeholder',
                        cursorAt: {left: 25}
                    });

                    $('#fundraiser-designations-groups form table:not("table.sticky-header")').sortable({
                        cancel: 'th, thead',
                        placeholder: 'row-sortable-placeholder',
                        items: 'tr:not("thead tr")',
                        cursorAt: {left: 25},
                        forcePlaceholderSize: true,
                        opacity: 0.5,
                        delay: 150,
                        start: function (event, ui) {
                        },
                        stop: function (event, ui) {
                            App.Handlers.reorderItems();
                        },
                    });

                    $('#fundraiser-designations-groups form').sortable({
                        placeholder: 'table-sortable-placeholder',
                        items: 'table:not("table.sticky-header")',
                        cursorAt: {left: 25},
                        tolerance: "pointer",
                        opacity: 0.5,
                        delay: 150,
                        start: function (event, ui) {
                            ui.placeholder.hide();
                        },
                        change: function (event, ui) {
                            ui.placeholder.hide();
                            ui.placeholder.show(250, 'linear');
                        },
                        stop: function (event, ui) {
                            App.Handlers.reorderItems();
                        },
                    });
                }

                App.Handlers.reorderItems();
            }); // End window.ready
        }
    }
})(jQuery);
